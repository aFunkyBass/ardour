/*
 * Copyright (C) 2005-2024 Paul Davis <paul@linuxaudiosystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "midi_cue_automation_line.h"

MidiCueAutomationLine::MidiCueAutomationLine (const std::string&                      name,
                                              EditingContext&                         ec,
                                              ArdourCanvas::Item&                     parent,
                                              ArdourCanvas::Rectangle*                drag_base,
                                              std::shared_ptr<ARDOUR::AutomationList> al,
                                              const ARDOUR::ParameterDescriptor&      desc)
: AutomationLineBase (name, ec, parent, drag_base, al, desc)
{
}

bool
MidiCueAutomationLine::event_handler (GdkEvent*)
{
	return true;
}
