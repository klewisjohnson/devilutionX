#pragma once

#define PS_CONNECTED 0x10000
#define PS_TURN_ARRIVED 0x20000
#define PS_ACTIVE 0x40000

#define LEAVE_NORMAL 3
#define LEAVE_ENDING 0x40000004

namespace dvlnet {
	class base : public dvlnet {
	public:
		base(buffer_t info);

		virtual int create(std::string addrstr, std::string passwd) = 0;
		virtual int join(std::string addrstr, std::string passwd) = 0;

		virtual bool SNetReceiveMessage(int* sender, char** data, int* size);
		virtual bool SNetSendMessage(int dest, void* data, unsigned int size);
		virtual bool SNetReceiveTurns(char** data, unsigned int* size,
		                              DWORD* status);
		virtual bool SNetSendTurn(char* data, unsigned int size);
		virtual int SNetGetProviderCaps(struct _SNETCAPS* caps);
		virtual void* SNetRegisterEventHandler(event_type evtype,
		                                       snet_event_func func);
		virtual void* SNetUnregisterEventHandler(event_type evtype,
		                                         snet_event_func func);
		virtual bool SNetLeaveGame(int type);

		virtual void poll() = 0;
		virtual void send(packet& pkt) = 0;

	protected:
		std::map<event_type, snet_event_func> registered_handlers;
		buffer_t game_init_info;

		struct message_t {
			int sender; // change int to something else in devilution code later
			buffer_t payload;
			message_t() : sender(-1), payload({}) {}
		    message_t(int s, buffer_t p) : sender(s), payload(p) {}
		};

		message_t message_last;
		std::queue<message_t> message_queue;
		std::array<turn_t, MAX_PLRS> turn_last = {};
		std::array<std::queue<turn_t>, MAX_PLRS> turn_queue;
		std::array<bool, MAX_PLRS> active_table = {};
		std::array<bool, MAX_PLRS> connected_table = {};

		plr_t plr_self = PLR_BROADCAST;
		cookie_t cookie_self = 0;

		std::unique_ptr<packet_factory> pktfty;

		void setup_password(std::string pw);
		void handle_accept(packet& pkt);
		void recv_local(packet& pkt);
		void run_event_handler(_SNETEVENT& ev);
	};
}