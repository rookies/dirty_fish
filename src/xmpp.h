/* src/xmpp.h
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

#ifndef __xmpp_h__
	#define __xmpp_h__
	#include <stdio.h>
	#include <loudmouth/loudmouth.h>
	#include "globals.h"
	#include "functions.h"
	
	int sendMessage (
		LmConnection *conn,
		const gchar *message,
		const gchar *to,
		GError **error
	);
	int sendToRoom (
		LmConnection *conn,
		const gchar *message,
		GError **error
	);
	int sendPostInfo (
		LmConnection *conn,
		const gchar *to,
		GError **error
	);
	LmHandlerResult messageHandler (
		LmMessageHandler *handler,
		LmConnection *conn,
		LmMessage *m,
		gpointer user_data
	);
	LmHandlerResult iqHandler (
		LmMessageHandler *handler,
		LmConnection *conn,
		LmMessage *m,
		gpointer user_data
	);
	LmHandlerResult presenceHandler (
		LmMessageHandler *handler,
		LmConnection *conn,
		LmMessage *m,
		gpointer user_data
	);
#endif //__xmpp_h__
