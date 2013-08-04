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
 *   Name: eventsmanager.cpp                                               *
 *   Date: 03/08/2013                                                      *
 ***************************************************************************/

#include <task/eventsmanager.h>

QList<EventsManager::EventListener> *EventsManager::eventListeners;

void EventsManager::init()
{
    eventListeners = new QList<EventListener>();
}

int EventsManager::connectEventListener(void *resource, ThreadControlBlock *thread, EventType filter)
{
    EventListener listener;
    listener.resource = resource;
    listener.thread = thread;
    listener.filter = filter;
    eventListeners->append(listener);
    return 0;
}

ThreadControlBlock *EventsManager::takeEventListener(void *resource, EventType filter)
{
    for (int i = 0; i < eventListeners->count(); i++){
        if ((eventListeners->at(i).resource == resource) && (eventListeners->at(i).filter & filter)){
            EventListener listener = eventListeners->takeAt(i);
            return listener.thread;
        }
    }
    return NULL;
}

bool EventsManager::disconnectEventListener(void *resource, ThreadControlBlock *thread)
{
    for (int i = 0; i < eventListeners->count(); i++){
        if ((eventListeners->at(i).thread == thread) && (eventListeners->at(i).resource == resource)){
            eventListeners->removeAt(i);
            return true;
        }
    }
    return false;
}

