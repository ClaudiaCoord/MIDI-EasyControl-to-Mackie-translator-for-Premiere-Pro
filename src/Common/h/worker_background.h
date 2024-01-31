/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	typedef std::function<void()> callWorkerCb;
	typedef std::pair<uint32_t, std::function<void()>> pairWorkerCb;
	typedef std::future<void> callFutureCb;

    class FLAG_EXPORT worker_background {
	private:
		common_timer t_{};
		std::forward_list<pairWorkerCb> list_{};

		void dispose_();
		void worker_();
		void pack_();

    public:
		worker_background();
		~worker_background();

		static worker_background& Get();
		void to_async(callFutureCb);

		void start(uint32_t = 0U);
		void stop();
		void clear();

		uint32_t set_interval(uint32_t);
		uint32_t add(callWorkerCb);
		uint32_t remove(uint32_t);
    };
}

