/*
Copyright (C) 2018-2020 Tom Schoonjans and Laszlo Vincze

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XMI_MSIM_GUI_LAYER_BOX_H
#define XMI_MSIM_GUI_LAYER_BOX_H

#include <gtk/gtk.h>
#include "xmi_data_structs.h"

G_BEGIN_DECLS

#define XMI_MSIM_GUI_TYPE_LAYER_BOX 		       (xmi_msim_gui_layer_box_get_type())
#define XMI_MSIM_GUI_LAYER_BOX(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), XMI_MSIM_GUI_TYPE_LAYER_BOX, XmiMsimGuiLayerBox))
#define XMI_MSIM_GUI_LAYER_BOX_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), XMI_MSIM_GUI_TYPE_LAYER_BOX, XmiMsimGuiLayerBoxClass))
#define XMI_MSIM_GUI_IS_LAYER_BOX(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XMI_MSIM_GUI_TYPE_LAYER_BOX))
#define XMI_MSIM_GUI_IS_LAYER_BOX_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), XMI_MSIM_GUI_TYPE_LAYER_BOX))
#define XMI_MSIM_GUI_LAYER_BOX_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), XMI_MSIM_GUI_TYPE_LAYER_BOX, XmiMsimGuiLayerBoxClass))

typedef struct _XmiMsimGuiLayerBox		XmiMsimGuiLayerBox;
typedef struct _XmiMsimGuiLayerBoxClass   	XmiMsimGuiLayerBoxClass;

typedef enum {
	XMI_MSIM_GUI_LAYER_BOX_TYPE_SAMPLE_COMPOSITION,
	XMI_MSIM_GUI_LAYER_BOX_TYPE_EXCITATION_ABSORBERS,
	XMI_MSIM_GUI_LAYER_BOX_TYPE_DETECTOR_ABSORBERS,
	XMI_MSIM_GUI_LAYER_BOX_TYPE_CRYSTAL_COMPOSITION,
} XmiMsimGuiLayerBoxType;

GtkWidget* xmi_msim_gui_layer_box_new(XmiMsimGuiLayerBoxType layers_type);

// this method does not emit a signal!!!
void xmi_msim_gui_layer_box_set_composition(XmiMsimGuiLayerBox *self, const xmi_composition *composition);

// to be used by the UndoManager
xmi_composition* xmi_msim_gui_layer_box_get_composition(XmiMsimGuiLayerBox *self);

// to be used by the ClipboardManager
void xmi_msim_gui_layer_box_clipboard_copy(XmiMsimGuiLayerBox *self);
void xmi_msim_gui_layer_box_clipboard_cut(XmiMsimGuiLayerBox *self);
void xmi_msim_gui_layer_box_clipboard_paste(XmiMsimGuiLayerBox *self);

XmiMsimGuiLayerBoxType xmi_msim_gui_layer_box_get_layers_type(XmiMsimGuiLayerBox *self);

GType xmi_msim_gui_layer_box_get_type(void) G_GNUC_CONST;

G_END_DECLS

#endif

