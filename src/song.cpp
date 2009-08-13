/***************************************************************************
 *   Copyright (C) 2008-2009 by Andrzej Rybczak                            *
 *   electricityispower@gmail.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <iomanip>
#include <sstream>

#include "charset.h"
#include "misc.h"
#include "song.h"

MPD::Song::Song(mpd_Song *s, bool copy_ptr) : 	itsSong(s ? s : mpd_newSong()),
						itsSlash(std::string::npos),
						itsHash(0),
						copyPtr(copy_ptr),
						isLocalised(0)
{
	if (itsSong->file)
		SetHashAndSlash();
}

MPD::Song::Song(const Song &s) : itsSong(0),
				itsNewName(s.itsNewName),
				itsSlash(s.itsSlash),
				itsHash(s.itsHash),
				copyPtr(s.copyPtr),
				isLocalised(s.isLocalised)
{
	itsSong = s.copyPtr ? s.itsSong : mpd_songDup(s.itsSong);
}

MPD::Song::~Song()
{
	if (itsSong)
		mpd_freeSong(itsSong);
}

std::string MPD::Song::GetLength() const
{
	return itsSong->time <= 0 ? "-:--" : ShowTime(itsSong->time);
}

void MPD::Song::Localize()
{
#	ifdef HAVE_ICONV_H
	if (isLocalised)
		return;
	str_pool_utf_to_locale(itsSong->file);
	SetHashAndSlash();
	str_pool_utf_to_locale(itsSong->artist);
	str_pool_utf_to_locale(itsSong->title);
	str_pool_utf_to_locale(itsSong->album);
	str_pool_utf_to_locale(itsSong->track);
	str_pool_utf_to_locale(itsSong->name);
	str_pool_utf_to_locale(itsSong->date);
	str_pool_utf_to_locale(itsSong->genre);
	str_pool_utf_to_locale(itsSong->composer);
	str_pool_utf_to_locale(itsSong->performer);
	str_pool_utf_to_locale(itsSong->disc);
	str_pool_utf_to_locale(itsSong->comment);
	isLocalised = 1;
#	endif // HAVE_ICONV_H
}

void MPD::Song::Clear()
{
	if (itsSong)
		mpd_freeSong(itsSong);
	itsSong = mpd_newSong();
	itsNewName.clear();
	itsSlash = std::string::npos;
	itsHash = 0;
	isLocalised = 0;
	copyPtr = 0;
}

bool MPD::Song::Empty() const
{
	return !itsSong || (!itsSong->file && !itsSong->title && !itsSong->artist && !itsSong->album && !itsSong->date && !itsSong->track && !itsSong->genre && !itsSong->composer && !itsSong->performer && !itsSong->disc && !itsSong->comment);
}

bool MPD::Song::isFromDB() const
{
	return (itsSong->file && itsSong->file[0] != '/') || itsSlash == std::string::npos;
}

bool MPD::Song::isStream() const
{
	return !strncmp(itsSong->file, "http://", 7);
}

std::string MPD::Song::GetFile() const
{
	return !itsSong->file ? "" : itsSong->file;
}

std::string MPD::Song::GetName() const
{
	if (itsSong->name)
		return itsSong->name;
	else if (!itsSong->file)
		return "";
	else if (itsSlash != std::string::npos)
		return itsSong->file+itsSlash+1;
	else
		return itsSong->file;
}

std::string MPD::Song::GetDirectory() const
{
	if (!itsSong->file || isStream())
		return "";
	else if (itsSlash == std::string::npos)
		return "/";
	else
		return std::string(itsSong->file, itsSlash);
}

std::string MPD::Song::GetArtist() const
{
	return !itsSong->artist ? "" : itsSong->artist;
}

std::string MPD::Song::GetTitle() const
{
	return !itsSong->title ? "" : itsSong->title;
}

std::string MPD::Song::GetAlbum() const
{
	return !itsSong->album ? "" : itsSong->album;
}

std::string MPD::Song::GetTrack() const
{
	if (!itsSong->track)
		return "";
	else if (itsSong->track[0] != '0' && !itsSong->track[1])
		return "0"+std::string(itsSong->track);
	else
		return itsSong->track;
}

std::string MPD::Song::GetDate() const
{
	return !itsSong->date ? "" : itsSong->date;
}

std::string MPD::Song::GetGenre() const
{
	return !itsSong->genre ? "" : itsSong->genre;
}

std::string MPD::Song::GetComposer() const
{
	return !itsSong->composer ? "" : itsSong->composer;
}

std::string MPD::Song::GetPerformer() const
{
	return !itsSong->performer ? "" : itsSong->performer;
}

std::string MPD::Song::GetDisc() const
{
	return !itsSong->disc ? "" : itsSong->disc;
}

std::string MPD::Song::GetComment() const
{
	return !itsSong->comment ? "" : itsSong->comment;
}

void MPD::Song::SetFile(const std::string &str)
{
	if (itsSong->file)
		str_pool_put(itsSong->file);
	itsSong->file = str.empty() ? 0 : str_pool_get(str.c_str());
	SetHashAndSlash();
}

void MPD::Song::SetArtist(const std::string &str)
{
	if (itsSong->artist)
		str_pool_put(itsSong->artist);
	itsSong->artist = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetTitle(const std::string &str)
{
	if (itsSong->title)
		str_pool_put(itsSong->title);
	itsSong->title = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetAlbum(const std::string &str)
{
	if (itsSong->album)
		str_pool_put(itsSong->album);
	itsSong->album = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetTrack(const std::string &str)
{
	if (itsSong->track)
		str_pool_put(itsSong->track);
	itsSong->track = str.empty() ? 0 : str_pool_get(IntoStr(StrToInt(str)).c_str());
}

void MPD::Song::SetTrack(int track)
{
	if (itsSong->track)
		str_pool_put(itsSong->track);
	itsSong->track = str_pool_get(IntoStr(track).c_str());
}

void MPD::Song::SetDate(const std::string &str)
{
	if (itsSong->date)
		str_pool_put(itsSong->date);
	itsSong->date = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetDate(int year)
{
	if (itsSong->date)
		str_pool_put(itsSong->date);
	itsSong->date = str_pool_get(IntoStr(year).c_str());
}

void MPD::Song::SetGenre(const std::string &str)
{
	if (itsSong->genre)
		str_pool_put(itsSong->genre);
	itsSong->genre = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetComposer(const std::string &str)
{
	if (itsSong->composer)
		str_pool_put(itsSong->composer);
	itsSong->composer = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetPerformer(const std::string &str)
{
	if (itsSong->performer)
		str_pool_put(itsSong->performer);
	itsSong->performer = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetDisc(const std::string &str)
{
	if (itsSong->disc)
		str_pool_put(itsSong->disc);
	itsSong->disc = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetComment(const std::string &str)
{
	if (itsSong->comment)
		str_pool_put(itsSong->comment);
	itsSong->comment = str.empty() ? 0 : str_pool_get(str.c_str());
}

void MPD::Song::SetPosition(int pos)
{
	itsSong->pos = pos;
}

std::string MPD::Song::Format_ParseBraces(std::string::const_iterator &it, std::string::const_iterator end_it) const
{
	std::string result;
	bool has_some_tags = 0;
	MPD::Song::GetFunction get = 0;
	while (*++it != '}')
	{
		while (*it == '{')
		{
			std::string tags = Format_ParseBraces(it, end_it);
			if (!tags.empty())
			{
				has_some_tags = 1;
				result += tags;
			}
		}
		if (*it == '}')
			break;
		else if (it == end_it)
			return "";
		
		if (*it == '%')
		{
			switch (*++it)
			{
				case 'l':
					get = &MPD::Song::GetLength;
					break;
				case 'D':
					get = &MPD::Song::GetDirectory;
					break;
				case 'f':
					get = &MPD::Song::GetName;
					break;
				case 'a':
					get = &MPD::Song::GetArtist;
					break;
				case 'b':
					get = &MPD::Song::GetAlbum;
					break;
				case 'y':
					get = &MPD::Song::GetDate;
					break;
				case 'n':
					get = &MPD::Song::GetTrack;
					break;
				case 'g':
					get = &MPD::Song::GetGenre;
					break;
				case 'c':
					get = &MPD::Song::GetComposer;
					break;
				case 'p':
					get = &MPD::Song::GetPerformer;
					break;
				case 'd':
					get = &MPD::Song::GetDisc;
					break;
				case 'C':
					get = &MPD::Song::GetComment;
					break;
				case 't':
					get = &MPD::Song::GetTitle;
					break;
				default:
					break;
			}
			if (get)
			{
				std::string tag = (this->*get)();
				if (!tag.empty() && (get != &MPD::Song::GetLength || GetTotalLength()))
				{
					has_some_tags = 1;
					result += tag;
				}
				else
					break;
			}
		}
		else
			result += *it;
	}
	if (*it != '}' || !has_some_tags)
	{
		for (; *it != '}'; ++it) { }
		if (*++it == '|')
			return Format_ParseBraces(++it, end_it);
		else
			return "";
	}
	else
	{
		if (*++it == '|')
			for (; *it != '}' || *++it == '|'; ++it) { }
		return result;
	}
}

std::string MPD::Song::toString(const std::string &format) const
{
	std::string result;
	for (std::string::const_iterator it = format.begin(); it != format.end(); ++it)
	{
		while (*it == '{')
			result += Format_ParseBraces(it, format.end());
		if (it == format.end())
			break;
		
		if (*it == '%')
		{
			switch (*++it)
			{
				case 'l':
					result += GetLength();
					break;
				case 'D':
					result += GetDirectory();
					break;
				case 'f':
					result += GetName();
					break;
				case 'a':
					result += GetArtist();
					break;
				case 'b':
					result += GetAlbum();
					break;
				case 'y':
					result += GetDate();
					break;
				case 'n':
					result += GetTrack();
					break;
				case 'g':
					result += GetGenre();
					break;
				case 'c':
					result += GetComposer();
					break;
				case 'p':
					result += GetPerformer();
					break;
				case 'd':
					result += GetDisc();
					break;
				case 'C':
					result += GetComment();
					break;
				case 't':
					result += GetTitle();
					break;
				default:
					break;
			}
		}
		else
			result += *it;
	}
	return result;
}

MPD::Song &MPD::Song::operator=(const MPD::Song &s)
{
	if (this == &s)
		return *this;
	if (itsSong)
		mpd_freeSong(itsSong);
	itsSong = s.copyPtr ? s.itsSong : mpd_songDup(s.itsSong);
	itsNewName = s.itsNewName;
	itsSlash = s.itsSlash;
	itsHash = s.itsHash;
	copyPtr = s.copyPtr;
	isLocalised = s.isLocalised;
	return *this;
}

std::string MPD::Song::ShowTime(int length)
{
	std::ostringstream ss;
	
	int hours = length/3600;
	length -= hours*3600;
	int minutes = length/60;
	length -= minutes*60;
	int seconds = length;
	
	if (hours > 0)
	{
		ss << hours << ":"
		<< std::setw(2) << std::setfill('0') << minutes << ":"
		<< std::setw(2) << std::setfill('0') << seconds;
	}
	else
	{
		ss << minutes << ":"
		<< std::setw(2) << std::setfill('0') << seconds;
	}
	return ss.str();
}

void MPD::Song::SetHashAndSlash()
{
	if (!itsSong->file)
		return;
	if (!isStream())
	{
		const char *tmp = strrchr(itsSong->file, '/');
		itsSlash = tmp && *(tmp-1) != '/' /* no http:// */ ? tmp-itsSong->file : std::string::npos;
	}
	itsHash = calc_hash(itsSong->file);
}

