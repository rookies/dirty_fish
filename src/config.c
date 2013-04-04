/* src/config.c
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

#include "config.h"

xmlXPathObjectPtr getNodeset (xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr ctx;
	xmlXPathObjectPtr res;
	
	ctx = xmlXPathNewContext(doc);
	if (ctx == NULL)
		return NULL;
	
	res = xmlXPathEvalExpression(xpath, ctx);
	xmlXPathFreeContext(ctx);
	if (res == NULL)
		return NULL;
	
	if(xmlXPathNodeSetIsEmpty(res->nodesetval))
	{
		xmlXPathFreeObject(res);
		return NULL;
	};
	return res;
}

gchar *configGetSimple (xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathObjectPtr res = getNodeset(doc, xpath);
	if (!res)
		return NULL;
	
	xmlNodeSetPtr nodeset = res->nodesetval;
	xmlChar *keyword;
	
	keyword = xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
	xmlXPathFreeObject(res);
	return (gchar *)keyword;
}

/*gchar **configGetArray (xmlDocPtr doc, xmlChar *xpath)
{
	gchar **ret;
	xmlXPathObjectPtr res = getNodeset(doc, xpath);
	if (!res)
		return NULL;
	
	xmlNodeSetPtr nodeset = res->nodesetval;
	xmlChar *keyword;
	int i;
	
	for (i=0; i < nodeset->nodeNr; i++)
	{
		keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
		ret[i] = keyword;
	}
	xmlXPathFreeObject(res);
	return ret;
}*/

int config_load (gchar *file, GError **error)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	
	// XML-Datei parsen:
	log_debug("config.c - Parsing configuration...");
	doc = xmlParseFile(file);
	if (doc == NULL)
	{
		g_set_error(
			error,
			g_quark_from_string("config_load"),
			100,
			"Document not parsed successfully."
		);
		return 1;
	};
	log_debug("config.c - Parsed configuration!");
	
	// root-Element extrahieren:
	log_debug("config.c - Extracting root-element...");
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		g_set_error(
			error,
			g_quark_from_string("config_load"),
			101,
			"Document is empty."
		);
		xmlFreeDoc(doc);
		return 1;
	};
	log_debug("config.c - Extracted root-element!");
	
	// testen, ob das root-Element auch den richtigen Namen hat:
	log_debug("config.c - Checking root-element name...");
	if (xmlStrcmp(cur->name, (const xmlChar *)"config"))
	{
		g_set_error(
			error,
			g_quark_from_string("config_load"),
			102,
			"root-element name has to be 'config', not '%s'.",
			cur->name
		);
		xmlFreeDoc(doc);
		return 1;
	};
	log_debug("config.c - Checked root-element name!");
	
	// jid (user & host):
	__config.user = g_strsplit(configGetSimple(doc, (xmlChar *)"/config/fm-bot/general/jid"), "@", 2)[0];
	__config.host = g_strsplit(configGetSimple(doc, (xmlChar *)"/config/fm-bot/general/jid"), "@", 2)[1];
	// password:
	__config.password = configGetSimple(doc, (xmlChar *)"/config/fm-bot/general/password");
	// resource:
	__config.resource = configGetSimple(doc, (xmlChar *)"/config/fm-bot/general/resource");
	// room:
	__config.room = configGetSimple(doc, (xmlChar *)"/config/fm-bot/general/room");
	// nick:
	__config.nick = configGetSimple(doc, (xmlChar *)"/config/fm-bot/general/nick");
	// roomJID:
	gchar tmp4[strlen(__config.room)*2+strlen(__config.nick)*2+20];
	g_sprintf(
		tmp4,
		"%s/%s",
		__config.room,
		__config.nick
	);
	__config.roomJID = g_strdup(tmp4);
	// clientName:
	__config.clientName = configGetSimple(doc, (xmlChar *)"/config/fm-bot/vcard/clientName");
	// clientVersion:
	__config.clientVersion = configGetSimple(doc, (xmlChar *)"/config/fm-bot/vcard/clientVersion");
	// os:
	__config.os = configGetSimple(doc, (xmlChar *)"/config/fm-bot/vcard/os");
	// secondBotURL:
	gchar *tmp1 = configGetSimple(doc, (xmlChar *)"/config/common/xmlrpc/host");
	gchar *tmp2 = configGetSimple(doc, (xmlChar *)"/config/common/xmlrpc/port");
	gchar tmp3[strlen(tmp1)+strlen(tmp2)+9];
	g_sprintf(
		tmp3,
		"http://%s:%d",
		tmp1,
		atoi(tmp2)
	);
	__config.secondBotURL = g_strdup(tmp3);
	// helpMessage:
	__config.helpMessage = configGetSimple(doc, (xmlChar *)"/config/fm-bot/messages/help");
	// mumbleMessage:
	__config.mumbleMessage = configGetSimple(doc, (xmlChar *)"/config/fm-bot/messages/mumble");
	/*gchar **djJIDs;
	gchar **djNicks;
	gchar **adminJIDs;
	gchar **adminNicks;
	gchar **barDrinks;
	gchar **barMessages1;
	gchar **barMessages2;*/
	
	// Aufräumen:
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}
