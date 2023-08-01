/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

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
		common_timer t__;
		uint32_t interval__;
		std::forward_list<pairWorkerCb> list__;

		void worker();
		void pack();

    public:
		worker_background();
		~worker_background();

		static worker_background& Get();
		void to_async(callFutureCb);

		void start();
		void stop();
		void clear();

		void set_interval(uint32_t);
		uint32_t add(callWorkerCb);
		void remove(uint32_t);
    };
}

