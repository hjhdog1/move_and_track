#pragma once

using namespace System;

namespace CPlusPlusEvent {
	ref class CanlibWaitEvent : public System::Threading::WaitHandle
	{
	public:
		CanlibWaitEvent(Object^ we);
	};
}