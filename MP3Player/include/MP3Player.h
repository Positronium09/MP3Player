#pragma once

#include <string>
#include <windows.h>

class MP3Player
{
	public:
#ifdef UNICODE
	using string = std::wstring;
#else
	using string = std::string;
#endif
	private:
	static constexpr size_t returnBufferLength = 128;
	bool isOpen = false;
	bool isPlaying = false;
	bool isRepeating = false;
	TCHAR* returnText = nullptr;
	string filename;
	string alias;

	public:
	MP3Player();
	MP3Player(const MP3Player&) = default;

	MP3Player(const string& filename);
	~MP3Player();

	MCIERROR Open(const string& filename);
	MCIERROR Close();

	MCIERROR Play();
	MCIERROR Resume();
	MCIERROR Pause();
	MCIERROR Stop();

	int GetPos();
	int GetLength();
	int GetVolume();

	const string& GetAlias();
	const string& GetFileName();

	MCIERROR SetPos(const int milliseconds, const bool resume = false);
	MCIERROR SetVolume(const int volume);

	MCIERROR Repeat(const bool enable);

	inline bool IsRepeating() { return isRepeating; };
	inline bool IsOpen() { return isOpen; }
	bool IsPlaying();
};
