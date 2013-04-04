/* src/main.c
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

// cc main.c config.c logging.c xmpp.c threads.c functions.c -o ../dirty_fish `pkg-config loudmouth-1.0 --cflags --libs` `pkg-config libxml-2.0 --cflags --libs` -lpthread `pkg-config libsoup-2.4 --libs --cflags`
#include "main.h"

int main (int argc, char **argv)
{
	// Kommandozeilenparameter verarbeiten:
	gchar *file;
	if (argc < 2)
	{
		g_fprintf(stderr, "Usage: %s [options] config.xml\n", argv[0]);
		return 1;
	}
	else
	{
		file = (gchar *)argv[argc-1];
	};
	
	// Variablen initialisieren:
	GMainLoop *loop;
	LmConnection *conn;
	GError *error = NULL;
	LmMessage *m;
	LmMessageHandler *mHandler_message;
	LmMessageHandler *mHandler_iq;
	LmMessageHandler *mHandler_presence;
	
	// Konfiguration laden:
	log_debug("main.c - Loading configuration...");
	if (config_load(file, &error) != 0)
	{
		g_fprintf(stderr, "Failed to load configuration from %s: %s\n", file, error->message);
		return 1;
	};
	log_debug("main.c - Loaded configuration!");
	
	// gtype initialisieren:
	g_type_init();
	
	// SoupSession starten:
	__soupSess = soup_session_sync_new();
	
	// Threads starten:
	log_debug("main.c - Starting scheduleThread...");
	pthread_create(&__scheduleThread, NULL, scheduleThread, NULL);
	log_debug("main.c - Started scheduleThread!");
	
	log_debug("main.c - Starting streaminfoThread...");
	pthread_create(&__streaminfoThread, NULL, streaminfoThread, NULL);
	log_debug("main.c - Started streaminfoThread!");
	
	// zum Jabber-Server verbinden:
	log_debug("main.c - Connecting to XMPP-server...");
	conn = lm_connection_new(__config.host);
	if (!lm_connection_open_and_block(conn, &error))
	{
		g_print("Verbindung zu '%s' konnte nicht hergestellt werden: %s\n", __config.host, error->message);
		return 1;
	};
	log_debug("main.c - Connected to XMPP-server!");
	
	// am Server authentifizieren:
	log_debug("main.c - Authenticating on XMPP-server...");
	if (!lm_connection_authenticate_and_block(conn, __config.user, __config.password, __config.resource, &error))
	{
		g_print("Authentifizierung als '%s' mit dem Passwort '%s' fehlgeschlagen: %s\n", __config.user, __config.password, error->message);
		return 1;
	};
	log_debug("main.c - Authenticated on XMPP-server!");
	
	// den message-Handler registrieren:
	log_debug("main.c - Registering messageHandler...");
	mHandler_message = lm_message_handler_new(messageHandler, NULL, NULL);
	lm_connection_register_message_handler(
		conn,
		mHandler_message,
		LM_MESSAGE_TYPE_MESSAGE,
		LM_HANDLER_PRIORITY_NORMAL
	);
	log_debug("main.c - Registered messageHandler!");
	
	// den iq-Handler registrieren:
	log_debug("main.c - Registering iqHandler...");
	mHandler_iq = lm_message_handler_new(iqHandler, NULL, NULL);
	lm_connection_register_message_handler(
		conn,
		mHandler_iq,
		LM_MESSAGE_TYPE_IQ,
		LM_HANDLER_PRIORITY_NORMAL
	);
	log_debug("main.c - Registered iqHandler!");
	
	// den presence-Handler registrieren:
	log_debug("main.c - Registering presenceHandler...");
	mHandler_presence = lm_message_handler_new(presenceHandler, NULL, NULL);
	lm_connection_register_message_handler(
		conn,
		mHandler_presence,
		LM_MESSAGE_TYPE_PRESENCE,
		LM_HANDLER_PRIORITY_NORMAL
	);
	log_debug("main.c - Registered presenceHandler!");
	
	// die presence zum Room senden:
	log_debug("main.c - Joining room...");
	m = lm_message_new(
		__config.roomJID,
		LM_MESSAGE_TYPE_PRESENCE
	);
	lm_message_node_add_child(m->node, "priority", "0");
	if (!lm_connection_send(conn, m, &error))
	{
		g_print("Fehler beim Senden der presence: %s\n", error->message);
		return 1;
	};
	lm_message_unref(m);
	log_debug("main.c - Joined room!");
	
	// Start definieren (zum Ignorieren der alten Nachrichten):
	__start = (int)time(NULL);
	
	// die Hauptschleife starten:
	log_debug("main.c - Starting mainloop...");
	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);
	
	// die Verbindung schließen:
	lm_connection_close(conn, NULL);
	lm_connection_unref(conn);
	
	return 0;
}
