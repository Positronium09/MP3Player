#include <iostream>

#include "MP3Player.h"


int main()
{
	bool file = 0;
	MP3Player mp3(TEXT("<file path>"));
	
	mp3.Play();
	mp3.Repeat(true);
	int volume = mp3.GetVolume();
	int length = mp3.GetLength();

	mp3.SetVolume(500);

	while (true)
	{
		int milliseconds = mp3.GetPos();

		int seconds = milliseconds / 1000;
		milliseconds %= 1000;

		int minutes = seconds / 60;
		seconds %= 60;

		int hours = minutes / 60;
		minutes %= 60;

		std::cout << hours << " Hours : " << minutes << " Minutes : " << seconds << " Seconds : " << milliseconds << " Milliseconds" << std::endl;

		if (GetAsyncKeyState('R') & 1)
		{
			mp3.Repeat(!mp3.IsRepeating());
		}

		if (GetAsyncKeyState(VK_SPACE) & 1)
		{
			if (mp3.IsPlaying())
			{
				mp3.Pause();
			}
			else
			{
				mp3.Resume();
			}
		}
		
		if ((!mp3.IsPlaying() && !mp3.IsRepeating()) || GetAsyncKeyState(VK_ESCAPE) & 1)
		{
			break;
		}
	}

	return 0;
}
