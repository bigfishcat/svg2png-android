/* svg_group.c: Data structures for SVG group elements
 
   Copyright � 2002 USC/Information Sciences Institute
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
  
   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
  
   Author: Carl Worth <cworth@isi.edu>
*/

#include "svgint.h"

static svg_status_t
_svg_group_grow_element_by (svg_group_t *group, int additional);

svg_status_t
_svg_group_init (svg_group_t *group)
{
    group->element = NULL;
    group->num_elements = 0;
    group->element_size = 0;

    _svg_length_init_unit (&group->width, 0, SVG_LENGTH_UNIT_PX, SVG_LENGTH_ORIENTATION_HORIZONTAL);
    _svg_length_init_unit (&group->height, 0, SVG_LENGTH_UNIT_PX, SVG_LENGTH_ORIENTATION_VERTICAL);
    group->view_box.aspect_ratio   = SVG_PRESERVE_ASPECT_RATIO_UNKNOWN;
    group->view_box.meet_or_slice = SVG_MEET_OR_SLICE_UNKNOWN;

    _svg_length_init_unit (&group->x, 0, SVG_LENGTH_UNIT_PX, SVG_LENGTH_ORIENTATION_HORIZONTAL);
    _svg_length_init_unit (&group->y, 0, SVG_LENGTH_UNIT_PX, SVG_LENGTH_ORIENTATION_VERTICAL);

    return SVG_STATUS_SUCCESS;
}

svg_status_t
_svg_group_deinit (svg_group_t *group, int group_is_use_element)
{
    int i;

    if(!group_is_use_element) {
	    for (i = 0; i < group->num_elements; i++)
		    _svg_element_destroy (group->element[i]);
    }
    
    free (group->element);
    group->element = NULL;
    group->num_elements = 0;
    group->element_size = 0;

    return SVG_STATUS_SUCCESS;
}

svg_status_t
_svg_group_add_element (svg_group_t *group, svg_element_t *element)
{
    svg_status_t status;

    if (group->num_elements >= group->element_size) {
	int additional = group->element_size ? group->element_size : 4;
	status = _svg_group_grow_element_by(group, additional);
	if (status)
	    return status;
    }

    group->element[group->num_elements] = element;
    group->num_elements++;

    return SVG_STATUS_SUCCESS;
}

void
_svg_group_drop_element (svg_group_t *group, svg_element_t *element) {
	int k;

	for(k = 0; k < group->num_elements; k++) {
		if(group->element[k] == element) {
			_svg_element_destroy(element);
			
			group->num_elements--;

			for(; k < group->num_elements; k++) {
				group->element[k] =
					group->element[k + 1];
			}
			group->element[k] = NULL;

			break;
		}
	}
}

svg_status_t _svg_group_init_copy (svg_group_t *group,
				   svg_group_t *other) {
	svg_status_t status;
	svg_element_t *clone;
	int i;
	group->element = NULL;
	group->num_elements = 0;
	group->element_size = 0;
	
	/* clone children */
	for (i=0; i < other->num_elements; i++) {
		status = _svg_element_clone (NULL, &clone, other->element[i]);
		if (status)
			return status;
		status = _svg_group_add_element (group, clone);
		if (status)
			return status;

		// point the parent correctly
		svg_element_t *group_e = container_of(group, svg_element_t, e.group);
		clone->parent = group_e;
	}
	
	group->width  = other->width;
	group->height = other->height;
	
	group->view_box = other->view_box;
	
	group->x = other->x;
	group->y = other->y;
	
	return SVG_STATUS_SUCCESS;
}

svg_status_t
_svg_group_render (svg_group_t		*group,
		   svg_render_engine_t	*engine,
		   void			*closure)
{
    int i;
    svg_status_t status, return_status = SVG_STATUS_SUCCESS;

    /* XXX: Perhaps this isn't the cleanest way to do this. It would
       be cleaner to just immediately abort on an error I think. In
       order to do that, we'd need to fix the parser so that it
       doesn't include images with null data in the tree for
       example. */
    for (i=0; i < group->num_elements; i++) {
	status = svg_element_render (group->element[i],
				     engine, closure);
	if (status == SVGINT_STATUS_ELEMENT_HAS_NO_PARENT) {
		_svg_group_drop_element (group, group->element[i]);
		i--; // a drop will result in that all following elements are moved to a lesser index, so we do this to not be off by one.
	} else {
		if (status && !return_status)
			return_status = status;
	}
    }

    return return_status;
}

svg_status_t
_svg_symbol_render (svg_element_t	*group,
		    svg_render_engine_t	*engine,
		    void		*closure)
{
    /* Never render a symbol directly. Only way to show a symbol is through <use>. */
    return SVG_STATUS_SUCCESS;
}

/* Apply attributes unique to `svg' elements */
svg_status_t
_svg_group_apply_svg_attributes (svg_group_t	*group,
				 const char	**attributes)
{
    const char *view_box_str, *aspect_ratio_str;
    svgint_status_t status;

    _svg_attribute_get_length (attributes, "width", &group->width, "100%");
    _svg_attribute_get_length (attributes, "height", &group->height, "100%");

    /* XXX: What else? */
    _svg_attribute_get_length (attributes, "x", &group->x, "0");
    _svg_attribute_get_length (attributes, "y", &group->y, "0");

    _svg_attribute_get_string (attributes, "viewBox", &view_box_str, NULL);

    if (view_box_str)
    {
	status = _svg_element_parse_view_box (view_box_str,
		    			      &group->view_box.box.x,
		    			      &group->view_box.box.y,
		    			      &group->view_box.box.width,
		    			      &group->view_box.box.height);

	group->view_box.aspect_ratio = SVG_PRESERVE_ASPECT_RATIO_NONE;
	_svg_attribute_get_string (attributes, "preserveAspectRatio", &aspect_ratio_str, NULL);
	if (aspect_ratio_str)
	    status = _svg_element_parse_aspect_ratio (aspect_ratio_str, &group->view_box);
    }

    return SVG_STATUS_SUCCESS;
}

/* Apply attributes common to `svg' and `g' elements */
svg_status_t
_svg_group_apply_group_attributes (svg_group_t		*group,
				   const char		**attributes)
{
    /* XXX: NYI */

    return SVG_STATUS_SUCCESS;
}

svg_status_t
_svg_group_apply_use_attributes (svg_element_t		*group,
				 const char		**attributes)
{
    const char *href;
    svg_element_t *ref;
    svg_element_t *clone;
    svgint_status_t status = SVG_STATUS_SUCCESS;

    _svg_attribute_get_string (attributes, "xlink:href", &href, "");
    _svg_fetch_element_by_id (group->doc, href + 1, &ref);
    if (!ref) {
	/* XXX: Should we report an error here? */
	return SVG_STATUS_SUCCESS;
    }

    _svg_attribute_get_length (attributes, "width", &group->e.group.width, "100%");
    _svg_attribute_get_length (attributes, "height", &group->e.group.height, "100%");

    clone = ref;
    _svg_element_reference(ref);
    
    if (status)
	return status;
    if (clone)
    {
	_svg_group_add_element (&group->e.group, clone);
    }

    return SVG_STATUS_SUCCESS;
}

static svg_status_t
_svg_group_grow_element_by (svg_group_t *group, int additional)
{
    svg_element_t **new_element;
    int old_size = group->element_size;
    int new_size = group->num_elements + additional;

    if (new_size <= group->element_size) {
	return SVG_STATUS_SUCCESS;
    }

    group->element_size = new_size;
    new_element = realloc (group->element,
			   group->element_size * sizeof(svg_element_t *));

    if (new_element == NULL) {
	group->element_size = old_size;
	return SVG_STATUS_NO_MEMORY;
    }

    group->element = new_element;

    return SVG_STATUS_SUCCESS;
}

svg_status_t
_svg_group_get_size (svg_group_t *group, svg_length_t *width, svg_length_t *height)
{
    *width = group->width;
    *height = group->height;

    return SVG_STATUS_SUCCESS;
}
