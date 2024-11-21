/*
 * Copyright (C) 2015 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2017 Robin Gareus <robin@gareus.org>
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

#ifndef _WIDGETS_TABBABLE_H_
#define _WIDGETS_TABBABLE_H_

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>

#include "gtkmm2ext/window_proxy.h"

#include "widgets/ardour_button.h"
#include "widgets/eventboxext.h"
#include "widgets/pane.h"
#include "widgets/visibility.h"

namespace Gtk {
	class Window;
	class Notebook;
}

namespace Gtkmm2ext {
	class VisibilityTracker;
}

namespace ArdourWidgets {

class LIBWIDGETS_API Tabbable : public Gtkmm2ext::WindowProxy
{
public:
	enum PaneLayout {
		NoPanes      = 0x00, ///< disable all attachment buttons, do not pack any panes or attachments
		PaneLeft     = 0x01, ///< left side attachment is a resizable pane
		PaneRight    = 0x02, ///< pack a resizable Pane on the right-side
		PaneBottom   = 0x04, ///< bottom Ebox is a resizable Pane
		AttLeft      = 0x08, ///< if PaneLeft is not set, pack a fixed size Ebox on the left (Editor-Mixer)
		AttBottom    = 0x10, ///< bottom is a fixed size EBox attachment
	};

	Tabbable (const std::string& user_visible_name, std::string const & untranslated_name, Gtk::Widget* top = NULL, bool tabbed_by_default = true, PaneLayout pl = PaneRight);
	~Tabbable ();

	void add_to_notebook (Gtk::Notebook& notebook);
	void make_visible ();
	void make_invisible ();
	void change_visibility ();
	void attach ();
	void detach ();

	Gtk::Widget& contents() const { return *_contents; }

	/* this is where ArdourUI packs the tab switchers
	 * (record/cues/edit/mix) into my toolbar area,
	 * in the case where I'm attached to the main window
	 */
	Gtk::EventBox& tab_btn_box () {return content_tabbables;}

	Gtk::Window* get (bool create = false);
	Gtk::Window* own_window () { return get (false); }
	virtual Gtk::Window* use_own_window (bool and_pack_it);

	void set_default_tabbed (bool yn);

	virtual void show_window ();

	bool window_visible () const;
	bool tabbed() const;
	bool tabbed_by_default () const;

	Gtk::Window* current_toplevel () const;

	Gtk::Notebook* tab_root_drop ();

	int set_state (const XMLNode&, int version);
	XMLNode& get_state () const;

	static std::string xml_node_name();

	sigc::signal1<void,Tabbable&> StateChange;

	void att_left_button_toggled();
	void att_right_button_toggled();
	void att_bottom_button_toggled();

protected:
	virtual void showhide_att_left (bool yn);
	virtual void showhide_att_right (bool yn);
	virtual void showhide_att_bottom (bool yn);

	bool delete_event_handler (GdkEventAny *ev);

	/* This is the heirarchy of a Tabbable's widget packing.
	 *
	 * The end result is to provide 7 event-boxes (marked with a $) where the tab can put its contents.
	 *
	 * +--_content_vbox----------------------------------------------------------------------------------+
	 * |                                                                                                 |
	 * | /--toolbar_frame------------------------------------------------------------------------------\ |
	 * | | +--content_header_hbox--------------------------------------------------------------------+ | |
	 * | | |                                                                                         | | |
	 * | | | +--content_app_bar--------------------+  +--attachment_hbox--+  +--content_tabbables--+ | | |
	 * | | | $                              (EBOX) |  |    (internal)     |  $              (EBOX) | | | |
	 * | | | |  MAIN APPLICATION BAR               |  | (attachment btns) |  | PAGE SWITCHER BTN   | | | |
	 * | | | |                                     |  |                   |  |                     | | | |
	 * | | | +-------------------------------------+  +-------------------+  +---------------------+ | | |
	 * | | |                                                                                         | | |
	 * | | +-----------------------------------------------------------------------------------------+ | |
	 * | \---------------------------------------------------------------------------------------------/ |
	 * |                                                                                                 |
	 * | +--content_hbox--OR--content_left_pane--(EXPAND|FILL)-----------------------------------------+ |
	 * | |                                                                                             | |
	 * | | +--att_left--+   +--content_midlevel_vbox--OR-content_midlevel_vpane--(EXPAND|FILL)-------+ | |
	 * | | $     (EBOX) |   | +--content_right_pane--(EXPAND|FILL)---------------------------------+ | | |
	 * | | |            |   | | +--content_inner_vbox-----------------+   +--content_right_vbox--+ | | | |
	 * | | |  O         |   | | |                                     |   |                      | | | | |
	 * | | |  P   S     |   | | | +--content_main_top---------------+ |   | +--att_right-------+ | | | | |
	 * | | |  T   I     |   | | | $    OPTIONAL TOOLBAR      (EBOX) | |   | $           (EBOX) | | | | | |
	 * | | |  I   D     |   | | | +---------------------------------+ |<->| |                  | | | | | |
	 * | | |  O   E     |<->| | |                                     | P | |  OPTIONAL        | | | | | |
	 * | | |  N   B     | O | | | +--content_main--(EXPAND|FILL)----+ | A | |  SIDEBAR         | | | | | |
	 * | | |  A   A     | P | | | $                          (EBOX) | | N | |                  | | | | | |
	 * | | |  L   R     | T | | | |                                 | | E | |                  | | | | | |
	 * | | |            | . | | | |    !!  MAIN PAGE CONTENT  !!    | |<->| |  (LIST)          | | | | | |
	 * | | |            | P | | | |                                 | |   | |                  | | | | | |
	 * | | |            | A | | | |                                 | |   | |                  | | | | | |
	 * | | |            | N | | | +---------------------------------+ |   | +------------------+ | | | | |
	 * | | |  (STRIP)   | E | | +-------------------------------------+   +----------------------+ | | | |
	 * | | |            |<->| +--------------------------------------------------------------------+ | | |
	 * | | |            |   |                          🡅  OPTIONAL  🡅                                | | |
	 * | | |            |   |                          🡇    PANE    🡇                                | | |
	 * | | |            |   | +-content_att_bottom-------------------------------------------------+ | | |
	 * | | |            |   | $                                                             (EBOX) | | | |
	 * | | |            |   | |   OPTIONAL BOTTOM (PROPERTIES)                                     | | | |
	 * | | |            |   | |                                                                    | | | |
	 * | | |            |   | +--------------------------------------------------------------------+ | | |
	 * | | +------------+   +------------------------------------------------------------------------+ | |
	 * | +---------------------------------------------------------------------------------------------+ |
	 * |                                                                                                 |
	 * +-------------------------------------------------------------------------------------------------+
	 *
	 */

	/* clang-format off */
	/*            _content_vbox                   * toplevel
	 *             toolbar_frame                  * the frame is managed in the implementation */
	Gtk::HBox       content_header_hbox;
	EventBoxExt       content_app_bar;           /* a placeholder for the transport bar, if you want one */
	Gtk::EventBox     content_attachments;       /* a placeholder the (strip, list, props) visibility buttons for this tab */
	Gtk::HBox           content_attachment_hbox;
	EventBoxExt       content_tabbables;         /* a placeholder for the tabbable switching buttons (used by ArdourUI) */
	HPane           content_left_pane;
	Gtk::HBox       content_hbox;
	EventBoxExt       content_att_left;          /* a placeholder for the mixer strip, if you want one */
	VPane             content_midlevel_vpane;
	Gtk::VBox         content_midlevel_vbox;
	HPane               content_right_pane;
	Gtk::VBox             content_inner_vbox;
	EventBoxExt             content_main_top;    /* a placeholder for the content-specific toolbar, if you want one */
	EventBoxExt             content_main;        /* a placeholder for the innermost content (recorder, cues, editor, mixer) */
	Gtk::VBox             content_right_vbox;
	EventBoxExt           content_att_right;     /* a placeholder for the sidebar list, if you want one */
	EventBoxExt         content_att_bottom;      /* a placeholder for the property box, if you want one */
	/* clang-format on */

	/* visibility controls */
	ArdourWidgets::ArdourButton left_attachment_button;
	ArdourWidgets::ArdourButton right_attachment_button;
	ArdourWidgets::ArdourButton bottom_attachment_button;

private:
	void default_layout ();
	void show_tab ();
	void hide_tab ();
	bool tab_close_clicked (GdkEventButton*);
	void show_own_window (bool and_pack_it);
	void window_mapped ();
	void window_unmapped ();

	Gtk::VBox      _content_vbox; /* this is the root widget for a full-featured tabbable, which contains:  */
	Gtk::Widget*   _contents; /* for most Tabbables this will be content_vbox;  but rc_options, for example, does something different. */
	Gtk::Notebook  _own_notebook;
	Gtk::Notebook* _parent_notebook;
	bool            tab_requested_by_state;
	PaneLayout     _panelayout;

};

} /* end namespace */

#endif
