#pragma once

namespace tactical
{
	class IApp
	{
	public:
		IApp();
		~IApp();

		void Run();
		
	protected:
		void GameLoop();
	};
}