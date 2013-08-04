/***************************************************************************
 *   Copyright 2013 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************
 *   Name: eventsmanager.h                                                 *
 *   Date: 04/08/2013                                                      *
 ***************************************************************************/

#ifndef _EVENTSMANAGER_H_
#define _EVENTSMANAGER_H_

#include <QList>
#include <task/threadcontrolblock.h>

class EventsManager
{
    public:
        enum ResourceType
        {
            VNode
        };

        enum EventType
        {
             NoEvent = 0,
             NewDataAvail = 1,
             ProcessTerminated = 2
        };

        struct EventListener
        {
            void *resource;
            ThreadControlBlock *thread;
            EventType filter;
        };

        static void init();
        static int connectEventListener(void *resource, ThreadControlBlock *thread, EventType filter);
        static ThreadControlBlock *takeEventListener(void *resource, EventType filter);
        static bool disconnectEventListener(void *resource, ThreadControlBlock *thread);

    private:
        static QList<EventListener> *eventListeners;
};

#endif

