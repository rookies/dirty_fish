/* src/functions.c
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

/**
 * http://www.java2s.com/Code/C/String/Findsubstringourownfunction.htm
*/
int find_substr (char *listPointer, char *itemPointer)
{
	int t;
	char *p, *p2;
	
	for (t=0; listPointer[t]; t++)
	{
		p = &listPointer[t];
		p2 = itemPointer;
		
		while (*p2 && *p2==*p)
		{
			p++;
			p2++;
		}
		if (!*p2)
			return t; /* 1st return */
	}
	return -1; /* 2nd return */
}
