/* src/globals.h
 * 
 * Copyright (C) 2010 Robert Knauer <robert AT fishmixx.de>
 * 
 * This file is part of dirty_fish.
 * 
 * dirty_fish is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3 only.
 * 
 * dirty_fish is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with dirty_fish. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __globals_h__
	#define __globals_h__
	
	#include <loudmouth/loudmouth.h>
	#include <libsoup/soup.h>
	
	/*typedef struct {
		gchar *jid;
		gchar *nick;
	} RoomRosterItem;*/
	
	int __start;
	pthread_t __scheduleThread;
	pthread_t __streaminfoThread;
	gchar *__schedule;
	gchar *__streaminfo;
	struct {
		gchar *host;
		gchar *user;
		gchar *password;
		gchar *resource;
		gchar *room;
		gchar *nick;
		gchar *roomJID;
		gchar **djJIDs;
		gchar **djNicks;
		gchar **adminJIDs;
		gchar **adminNicks;
		gchar *clientName;
		gchar *clientVersion;
		gchar *os;
		gchar *secondBotURL;
		gchar *helpMessage;
		gchar *mumbleMessage;
		gchar **barDrinks;
		gchar **barMessages1;
		gchar **barMessages2;
	} __config;
	SoupSession *__soupSess;
	//RoomRosterItem **__roomRoster;
	//int __roomRosterLength = 0;
#endif //__globals_h__
