/*
 * Copyright (C) 2006-2015 David Robillard <d@drobilla.net>
 * Copyright (C) 2008-2012 Hans Baier <hansfbaier@googlemail.com>
 * Copyright (C) 2008-2017 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2009-2012 Carl Hetherington <carl@carlh.net>
 * Copyright (C) 2015-2016 Tim Mayberry <mojofunk@gmail.com>
 * Copyright (C) 2015-2017 Nick Mainsbridge <mainsbridge@gmail.com>
 * Copyright (C) 2015-2017 Robin Gareus <robin@gareus.org>
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

#pragma once

#include <map>

#include "ardour/types.h"

#include "midi_view.h"

class VelocityDisplay;
class MidiCueAutomationLine;

namespace ArdourCanvas {
	class Box;
}

class MidiCueView : public MidiView
{
  public:
	MidiCueView (std::shared_ptr<ARDOUR::MidiTrack> mt,
	             uint32_t                    slot_index,
	             ArdourCanvas::Item&         parent,
	             ArdourCanvas::Item&         noscroll_parent,
	             EditingContext&             ec,
	             MidiViewBackground&         bg,
	             uint32_t                    basic_color
		);

	~MidiCueView();

	bool canvas_event (GdkEvent*);
	void set_samples_per_pixel (double);
	void set_height (double);

	void clear_ghost_events();
	void ghosts_model_changed();
	void ghosts_view_changed();
	void ghost_remove_note (NoteBase*);
	void ghost_add_note (NoteBase*);
	void ghost_sync_selection (NoteBase*);

	void update_automation_display (Evoral::Parameter const & param, ARDOUR::SelectionOperation);

	ArdourCanvas::Item* drag_group() const;

	std::list<SelectableOwner*> selectable_owners();
	MergeableLine* make_merger ();

	bool automation_rb_click (GdkEvent*, Temporal::timepos_t const &);
	void line_drag_click (GdkEvent*, Temporal::timepos_t const &);

	void automation_entry();
	void automation_leave ();

  protected:
	bool scroll (GdkEventScroll* ev);

	ArdourCanvas::Rectangle* automation_group;

	typedef std::shared_ptr<MidiCueAutomationLine>  CueAutomationLine;
	typedef std::shared_ptr<ARDOUR::AutomationControl>  CueAutomationControl;

	struct AutomationDisplayState {

		AutomationDisplayState (CueAutomationControl ctl, CueAutomationLine ln, bool vis)
			: control (ctl), line (ln), velocity_display (nullptr), visible (vis) {}
		AutomationDisplayState (VelocityDisplay& vdisp, bool vis)
			: control (nullptr), line (nullptr), velocity_display (&vdisp), visible (vis) {}

		~AutomationDisplayState();

		CueAutomationControl control;
		CueAutomationLine line;
		VelocityDisplay* velocity_display;
		bool visible;

		void hide ();
		void show ();
		void set_height (double);
	};

	typedef std::map<Evoral::Parameter, AutomationDisplayState> CueAutomationMap;

	CueAutomationMap automation_map;
	AutomationDisplayState* active_automation;

	VelocityDisplay* velocity_display;

	std::shared_ptr<Temporal::TempoMap const> tempo_map;
	ArdourCanvas::Rectangle* event_rect;
	uint32_t _slot_index;

	void update_sustained (Note *);
	void update_hit (Hit *);

	double _height;
};
