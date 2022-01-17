#include "MP3Player.h"

#include <memory>
#include <algorithm>
#include <strsafe.h>

#pragma comment (lib, "winmm.lib")


inline int GetNumberLengthInChars(const ULONGLONG num) { return snprintf(NULL, 0, "%lld", num); }

int ConvertStrToInt(const TCHAR* text, int radix = 10)
{
#ifdef UNICODE
#define stol std::wcstol
#else
#define stol std::strtol
#endif
	return stol(text, NULL, radix);

#undef stol
}

size_t GetStrLength(const TCHAR* text)
{
	size_t length = 0;
	(void)StringCchLength(text, STRSAFE_MAX_CCH, &length);

	return length + 1;
}

MP3Player::MP3Player() :
	returnText(new TCHAR[returnBufferLength])
{

}

MP3Player::MP3Player(const string& filename) :
	returnText(new TCHAR[returnBufferLength]), filename(filename)
{
	Open(filename);
}

MP3Player::~MP3Player()
{
	Close();
	delete[] returnText;
}

MCIERROR MP3Player::Open(const string& filename)
{
	MCIERROR error = 0;

#pragma region OPEN_COMMAND
	constexpr TCHAR openFormatLength[] = TEXT("open \"\" type mpegvideo alias ");
	constexpr TCHAR openFormat[] = TEXT("open \"%s\" type mpegvideo alias %s");
	if (IsOpen())
	{
		Close();
	}
	alias = filename;
	std::replace(alias.begin(), alias.end(), TEXT(' '), TEXT('_'));

	size_t textLength = GetStrLength(openFormatLength) + alias.length() * 2;
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, openFormat, filename.c_str(), alias.c_str());
	error = mciSendString(text.get(), NULL, NULL, NULL);
#pragma endregion

#pragma region SET_TIME_COMMAND
	constexpr TCHAR setTimeFormatLength[] = TEXT("set  time format milliseconds");
	constexpr TCHAR setTimeFormat[] = TEXT("set %s time format milliseconds");

	textLength = GetStrLength(setTimeFormatLength) + alias.length();
	text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, setTimeFormat, alias.c_str());
	error = mciSendString(text.get(), NULL, NULL, NULL);

#pragma endregion

	isOpen = error == 0;
	return error;
}

MCIERROR MP3Player::Close()
{
	MCIERROR error = 0;
	if (IsOpen())
	{
		constexpr TCHAR closeFormatLength[] = TEXT("close ");
		constexpr TCHAR closeFormat[] = TEXT("close %s");
		
		size_t textLength = GetStrLength(closeFormatLength) + alias.length();
		auto text = std::make_unique<TCHAR[]>(textLength);

		StringCchPrintf(text.get(), textLength, closeFormat, alias.c_str());
		error = mciSendString(text.get(), NULL, NULL, NULL);
		isOpen = isOpen && !(error == 0);
	}
	return error;
}

MCIERROR MP3Player::Play()
{
	MCIERROR error = 0;

	if (IsRepeating())
	{
		constexpr TCHAR playFormatLength[] = TEXT("play  repeat");
		constexpr TCHAR playFormat[] = TEXT("play %s repeat");

		size_t textLength = GetStrLength(playFormatLength) + alias.length();
		auto text = std::make_unique<TCHAR[]>(textLength);

		StringCchPrintf(text.get(), textLength, playFormat, alias.c_str());
		error = mciSendString(text.get(), NULL, NULL, NULL);
	}
	else
	{
		constexpr TCHAR playFormatLength[] = TEXT("play ");
		constexpr TCHAR playFormat[] = TEXT("play %s");

		size_t textLength = GetStrLength(playFormatLength) + alias.length();
		auto text = std::make_unique<TCHAR[]>(textLength);

		StringCchPrintf(text.get(), textLength, playFormat, alias.c_str());
		error = mciSendString(text.get(), NULL, NULL, NULL);
	}

	isPlaying = isPlaying || (error == 0);
	return error;
}

MCIERROR MP3Player::Resume()
{
	MCIERROR error = 0;

	constexpr TCHAR playFormatLength[] = TEXT("resume ");
	constexpr TCHAR playFormat[] = TEXT("resume %s");

	size_t textLength = GetStrLength(playFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, playFormat, alias.c_str());
	error = mciSendString(text.get(), NULL, NULL, NULL);

	isPlaying = isPlaying || (error == 0);

	return error;
}

MCIERROR MP3Player::Pause()
{
	MCIERROR error = 0;

	constexpr TCHAR pauseFormatLength[] = TEXT("pause ");
	constexpr TCHAR pauseFormat[] = TEXT("pause %s");

	size_t textLength = GetStrLength(pauseFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, pauseFormat, alias.c_str());
	error = mciSendString(text.get(), NULL, NULL, NULL);

	isPlaying = isPlaying && !(error == 0);

	return error;
}

MCIERROR MP3Player::Stop()
{
	MCIERROR error = 0;

	constexpr TCHAR stopFormatLength[] = TEXT("stop ");
	constexpr TCHAR stopFormat[] = TEXT("stop %s");

	size_t textLength = GetStrLength(stopFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, stopFormat, alias.c_str());
	error = mciSendString(text.get(), NULL, NULL, NULL);

	isPlaying = isPlaying && !(error == 0);

	return error;
}

int MP3Player::GetPos()
{
	int pos = -1;

	constexpr TCHAR positionFormatLength[] = TEXT("status  position");
	constexpr TCHAR positionFormat[] = TEXT("status %s position");

	size_t textLength = GetStrLength(positionFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, positionFormat, alias.c_str());

	MCIERROR error = mciSendString(text.get(), returnText, returnBufferLength, NULL);

	pos = ConvertStrToInt(returnText);
	
	return pos;
}

int MP3Player::GetLength()
{
	int length = -1;

	constexpr TCHAR statusFormatLength[] = TEXT("status  length");
	constexpr TCHAR statusFormat[] = TEXT("status %s length");

	size_t textLength = GetStrLength(statusFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, statusFormat, alias.c_str());

	mciSendString(text.get(), returnText, returnBufferLength, NULL);

	length = ConvertStrToInt(returnText);

	return length;
}

int MP3Player::GetVolume()
{
	int volume = -1;

	constexpr TCHAR volumeFormatLength[] = TEXT("status  volume");
	constexpr TCHAR volumeFormat[] = TEXT("status %s volume");

	size_t textLength = GetStrLength(volumeFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, volumeFormat, alias.c_str());

	mciSendString(text.get(), returnText, returnBufferLength, NULL);

	volume = ConvertStrToInt(returnText);

	return volume;
}

const MP3Player::string& MP3Player::GetAlias()
{
	return alias;
}

const MP3Player::string& MP3Player::GetFileName()
{
	return filename;
}


MCIERROR MP3Player::SetPos(const int milliseconds, const bool resume)
{
	MCIERROR error = 0;

	constexpr TCHAR seekFormatLength[] = TEXT("seek  to ");
	constexpr TCHAR seekFormat[] = TEXT("seek %s to %d");

	size_t textLength = GetStrLength(seekFormatLength) + alias.length() + GetNumberLengthInChars(milliseconds);
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, seekFormat, alias.c_str(), milliseconds);

	error = mciSendString(text.get(), NULL, NULL, NULL);

	if (resume)
	{
		error = Play();
	}

	return error;
}

MCIERROR MP3Player::SetVolume(const int volume)
{
	MCIERROR error = 0;

	constexpr TCHAR setaudioFormatLength[] = TEXT("setaudio  volume to ");
	constexpr TCHAR setaudioFormat[] = TEXT("setaudio %s volume to %d");

	size_t textLength = GetStrLength(setaudioFormatLength) + alias.length() + GetNumberLengthInChars(volume);
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, setaudioFormat, alias.c_str(), volume);

	error = mciSendString(text.get(), NULL, NULL, NULL);

	return error;
}

MCIERROR MP3Player::Repeat(const bool enable)
{
	isRepeating = enable;
	if (IsPlaying())
	{
		return Play();
	}
	return 0;
}

bool MP3Player::IsPlaying()
{
	constexpr TCHAR statusFormatLength[] = TEXT("status  mode");
	constexpr TCHAR statusFormat[] = TEXT("status %s mode");

	size_t textLength = GetStrLength(statusFormatLength) + alias.length();
	auto text = std::make_unique<TCHAR[]>(textLength);

	StringCchPrintf(text.get(), textLength, statusFormat, alias.c_str());

	mciSendString(text.get(), returnText, returnBufferLength, NULL);

	return std::memcmp(returnText, TEXT("playing"), 8) == 0;
}
