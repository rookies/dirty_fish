/* src/logging.h
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

#ifndef __logging_h__
	#define __logging_h__
	
	#include <stdio.h>
	#include <loudmouth/loudmouth.h>
	#include <time.h>
	
	void log_debug (gchar *message);
	//void log_error (gchar *message);
#endif //__logging_h__
