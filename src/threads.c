/* src/threads.c
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

#include "threads.h"

void *scheduleThread (void *dummy)
{
	SoupMessage *msg;
	gint status;
	while (1)
	{
		log_debug("threads.c - Starting new scheduleCycle...");
		msg = soup_message_new("GET", "http://www.fishmixx.de/programm/today.txt");
		status = soup_session_send_message(__soupSess, msg);
		if (status == 200)
			__schedule = g_strstrip(msg->response_body->data);
		else
			__schedule = "- Fehler beim Request auf den Sendeplan -";
		log_debug("threads.c - scheduleCycle END => sleeping");
		sleep(60);
	}
	return NULL;
}

void *streaminfoThread (void *dummy)
{
	SoupMessage *msg;
	gint status;
	while (1)
	{
		log_debug("threads.c - Starting new streaminfoCycle...");
		msg = soup_message_new("GET", "http://www.fishmixx.de/radio_stats/data.php");
		status = soup_session_send_message(__soupSess, msg);
		if (status == 200)
			__streaminfo = g_strstrip(msg->response_body->data);
		else
			__streaminfo = "- Fehler beim Request auf die API -";
		log_debug("threads.c - streaminfoCycle END => sleeping");
		sleep(10);
	}
	return NULL;
}
