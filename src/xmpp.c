/* src/xmpp.c
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

#include "xmpp.h"

int sendMessage (
	LmConnection *conn,
	const gchar *message,
	const gchar *to,
	GError **error
)
{
	int status = 0;
	error = NULL;
	LmMessage *m;
	m = lm_message_new_with_sub_type(
		to,
		LM_MESSAGE_TYPE_MESSAGE,
		LM_MESSAGE_SUB_TYPE_CHAT
	);
	lm_message_node_add_child(m->node, "body", message);
	if (!lm_connection_send(conn, m, error))
		status = 1;
	lm_message_unref(m);
	return status;
}

int sendToRoom (
	LmConnection *conn,
	const gchar *message,
	GError **error
)
{
	int status = 0;
	error = NULL;
	LmMessage *m;
	m = lm_message_new_with_sub_type(
		__config.room,
		LM_MESSAGE_TYPE_MESSAGE,
		LM_MESSAGE_SUB_TYPE_GROUPCHAT
	);
	lm_message_node_add_child(m->node, "body", message);
	if (!lm_connection_send(conn, m, error))
		status = 1;
	lm_message_unref(m);
	return status;
}

int sendPostInfo (
	LmConnection *conn,
	const gchar *to,
	GError **error
)
{
	gchar tmp[strlen(to)+20];
	g_sprintf(
		tmp,
		"%s, du hast Post! ;-)",
		to
	);
	sendToRoom(conn, tmp, &error);
}

LmHandlerResult messageHandler (
	LmMessageHandler *handler,
	LmConnection *conn,
	LmMessage *m,
	gpointer user_data
)
{
	log_debug("messageHandler() START");
	// alte Nachrichten ignorieren:
	if (__start+2 > (int)time(NULL))
	{
		log_debug("messageHandler() END (Reason: old)");
		return LM_HANDLER_RESULT_REMOVE_MESSAGE;
	};
	// eigene Nachrichten ignorieren:
	if (g_strcmp0(lm_message_node_get_attribute(m->node, "from"), __config.roomJID) == 0)
	{
		log_debug("messageHandler() END (Reason: own)");
		return LM_HANDLER_RESULT_REMOVE_MESSAGE;
	};
	if (lm_message_get_sub_type(m) == LM_MESSAGE_SUB_TYPE_GROUPCHAT)
	{
		// !sendeplan-Kommando:
		if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 10), "!sendeplan") == 0)
		{
			log_debug("!sendeplan START");
			GError *error;
			sendMessage(
				conn,
				__schedule,
				lm_message_node_get_attribute(m->node, "from"),
				&error
			);
			sendPostInfo(conn, g_strsplit(lm_message_node_get_attribute(m->node, "from"), "/", 2)[1], &error);
			log_debug("!sendeplan END");
		}
		// !help-Kommando:
		else if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 5), "!help") == 0)
		{
			log_debug("!help START");
			GError *error;
			sendMessage(
				conn,
				__config.helpMessage,
				lm_message_node_get_attribute(m->node, "from"),
				&error
			);
			sendPostInfo(conn, g_strsplit(lm_message_node_get_attribute(m->node, "from"), "/", 2)[1], &error);
			log_debug("!help END");
		}
		// !stream-Kommando:
		else if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 7), "!stream") == 0)
		{
			log_debug("!stream START");
			GError *error;
			gchar tmp[strlen(lm_message_node_get_attribute(m->node, "from"))+40];
			if (g_strcmp0(__streaminfo, "0") == 0)
			{
				g_sprintf(
					tmp,
					"%s, gerade läuft leider keine Sendung :(",
					g_strsplit(lm_message_node_get_attribute(m->node, "from"), "/", 2)[1]
				);
				sendToRoom(conn, tmp, &error);
			}
			else if (find_substr(__streaminfo, "**") == -1)
			{
				sendToRoom(conn, "Es ist leider ein Fehler aufgetreten :(", &error);
			}
			else
			{
				gchar **tmp1;
				tmp1 = g_strsplit(__streaminfo, "**", 5);
				
				gchar tmp2[strlen(__streaminfo)+140];
				g_sprintf(
					tmp2,
					"%s, gerade läuft „%s“ von „%s“ in der Sendung „%s“ mit „%s“ und %d %s: http://stream.fishmixx.de:8000/fishmixx.ogg.m3u",
					g_strsplit(lm_message_node_get_attribute(m->node, "from"), "/", 2)[1],
					tmp1[1],
					tmp1[0],
					tmp1[3],
					tmp1[4],
					atoi(tmp1[2]),
					((atoi(tmp1[2])==1)?"Hörer":"Hörern")
				);
				sendToRoom(conn, tmp2, &error);
			};
			log_debug("!stream END");
		}
		// !mumble-Kommando:
		else if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 7), "!mumble") == 0)
		{
			log_debug("!mumble START");
			GError *error;
			if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 8), "!mumble ") == 0)
			{
				// Name mit angegeben!
				gchar *name;
				name = g_strstrip(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 8);
				if (g_strcmp0(name, "") != 0)
				{
					gchar tmp[strlen(__config.mumbleMessage)+strlen(name)+5];
					g_sprintf(
						tmp,
						"%s: %s",
						name,
						__config.mumbleMessage
					);
					sendToRoom(conn, tmp, &error);
				}
			}
			else
				sendToRoom(conn, __config.mumbleMessage, &error);
			log_debug("!mumble END");
		};
	}
	else
	{
		// sendeplan-Kommando:
		if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 9), "sendeplan") == 0)
		{
			log_debug("sendeplan START");
			GError *error;
			sendMessage(
				conn,
				__schedule,
				lm_message_node_get_attribute(m->node, "from"),
				&error
			);
			log_debug("sendeplan END");
		}
		// help-Kommando:
		else if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 4), "help") == 0)
		{
			log_debug("help START");
			GError *error;
			sendMessage(
				conn,
				__config.helpMessage,
				lm_message_node_get_attribute(m->node, "from"),
				&error
			);
			log_debug("help END");
		}
		// stream-Kommando:
		else if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 6), "stream") == 0)
		{
			log_debug("stream START");
			GError *error;
			gchar tmp[strlen(lm_message_node_get_attribute(m->node, "from"))+40];
			if (g_strcmp0(__streaminfo, "0") == 0)
			{
				sendMessage(
					conn,
					"Gerade läuft leider keine Sendung :(",
					lm_message_node_get_attribute(m->node, "from"),
					&error
				);
			}
			else if (find_substr(__streaminfo, "**") == -1)
			{
				sendMessage(
					conn,
					"Es ist leider ein Fehler aufgetreten :(",
					lm_message_node_get_attribute(m->node, "from"),
					&error
				);
			}
			else
			{
				gchar **tmp1;
				tmp1 = g_strsplit(__streaminfo, "**", 5);
				
				gchar tmp2[strlen(__streaminfo)+140];
				g_sprintf(
					tmp2,
					"Gerade läuft „%s“ von „%s“ in der Sendung „%s“ mit „%s“ und %d %s: http://stream.fishmixx.de:8000/fishmixx.ogg.m3u",
					tmp1[1],
					tmp1[0],
					tmp1[3],
					tmp1[4],
					atoi(tmp1[2]),
					((atoi(tmp1[2])==1)?"Hörer":"Hörern")
				);
				sendMessage(
					conn,
					tmp2,
					lm_message_node_get_attribute(m->node, "from"),
					&error
				);
			};
			log_debug("stream END");
		}
		// mumble-Kommando:
		else if (g_strcmp0(g_strndup(lm_message_node_get_value(lm_message_node_get_child(m->node, "body")) + 0, 6), "mumble") == 0)
		{
			log_debug("mumble START");
			GError *error;
			sendMessage(
				conn,
				__config.mumbleMessage,
				lm_message_node_get_attribute(m->node, "from"),
				&error
			);
			log_debug("mumble END");
		};
	};
	log_debug("messageHandler() END");
	// Nachricht als erledigt markieren:
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
}

LmHandlerResult iqHandler (
	LmMessageHandler *handler,
	LmConnection *conn,
	LmMessage *m,
	gpointer user_data
)
{
	log_debug("iqHandler() START");
	if (lm_message_get_sub_type(m) == LM_MESSAGE_SUB_TYPE_GET)
	{
		// der IQ-Request ist vom Typ "get"
		if (g_strcmp0(lm_message_node_get_attribute(lm_message_node_get_child(m->node, "query"), "xmlns"), "jabber:iq:version") == 0)
		{
			// der IQ-Request ist ein version-Request => Antwort senden:
			LmMessage *m2;
			GError *error = NULL;
			LmMessageNode *query;
			// neues IQ-Paket bilden:
			m2 = lm_message_new_with_sub_type(
				lm_message_node_get_attribute(m->node, "from"),
				LM_MESSAGE_TYPE_IQ,
				LM_MESSAGE_SUB_TYPE_RESULT
			);
			// die alte ID anhängen:
			lm_message_node_set_attribute(m2->node, "id", lm_message_node_get_attribute(m->node, "id"));
			// ein query-Element erstellen:
			query = lm_message_node_add_child(m2->node, "query", "");
			// ein name-Element mit dem Client-Namen erstellen:
			lm_message_node_add_child(query, "name", __config.clientName);
			// ein version-Element mit der Client-Version erstellen:
			lm_message_node_add_child(query, "version", __config.clientVersion);
			// ein os-Element mit dem Betriebssystem erstellen:
			lm_message_node_add_child(query, "os", __config.os);
			// Senden:
			lm_connection_send(conn, m2, &error);
			// Nachricht löschen:
			lm_message_unref(m2);
		};
	};
	log_debug("iqHandler() END");
	// Nachricht als erledigt markieren:
	return LM_HANDLER_RESULT_REMOVE_MESSAGE;
}

LmHandlerResult presenceHandler (
	LmMessageHandler *handler,
	LmConnection *conn,
	LmMessage *m,
	gpointer user_data
)
{
	log_debug("presenceHandler() START");
	int i;
	if (g_strcmp0(g_strndup(lm_message_node_get_attribute(m->node, "from") + 0, strlen(__config.room)), __config.room) != 0)
	{
		log_debug("presenceHandler() END (Reason: external)");
		return LM_HANDLER_RESULT_REMOVE_MESSAGE;
	};
	LmMessageNode *x;
	LmMessageNode *item;
	gchar *jid;
	x = lm_message_node_get_child(m->node, "x");
	while (1)
	{
		if (g_strcmp0(lm_message_node_get_attribute(x, "xmlns"), "http://jabber.org/protocol/muc#user") == 0)
			break;
		else
			x = x->next;
	}
	item = lm_message_node_get_child(x, "item");
	g_print("Nick: %s\n", g_strndup(lm_message_node_get_attribute(m->node, "from") + strlen(__config.room)+1, strlen(lm_message_node_get_attribute(m->node, "from"))));
	g_print("JID: %s\n", g_strsplit(lm_message_node_get_attribute(item, "jid"), "/", 2)[0]);
	if (lm_message_get_sub_type(m) == LM_MESSAGE_SUB_TYPE_UNAVAILABLE)
		g_print("  => LEAVE\n");
	else
	{
		// Join
		g_print("  => JOIN\n");
		// Testen, ob es die JID-Nick-Kombination im Roster schon gibt:
		/*for (i=0; i < __roomRosterLength; i++)
		{
			if(g_strcmp0(__roomRoster[i]->jid, g_strsplit(lm_message_node_get_attribute(item, "jid"), "/", 2)[0]) == 0
			   && g_strcmp0(__roomRoster[i]->nick, g_strndup(lm_message_node_get_attribute(m->node, "from") + strlen(__config.room)+1, strlen(lm_message_node_get_attribute(m->node, "from")))) == 0)
				g_print(" => NOT NEW\n");
		}*/
	};
	g_print("---------------------------\n");
	log_debug("presenceHandler() END");
}
