internal void
FleuryDoTheCursorInterpolation(Application_Links *app, Frame_Info frame_info,
                               Rect_f32 *rect, Rect_f32 *last_rect, Rect_f32 target)
{
    *last_rect = *rect;
    
    float x_change = target.x0 - rect->x0;
    float y_change = target.y0 - rect->y0;
    
    float cursor_size_x = (target.x1 - target.x0) * (1 + fabsf(x_change) / 180.f);
    float cursor_size_y = (target.y1 - target.y0) * (1 + fabsf(y_change) / 120.f);
    
    b32 should_animate_cursor = !global_battery_saver;
    if(should_animate_cursor)
    {
        if(fabs(x_change) > 1.f || fabs(y_change) > 1.f)
        {
            animate_in_n_milliseconds(app, 0);
        }
    }
    else
    {
        *rect = *last_rect = target;
        cursor_size_y = target.y1 - target.y0;
    }
    
    if(should_animate_cursor)
    {
        rect->x0 += (x_change) * frame_info.animation_dt * 30.f;
        rect->y0 += (y_change) * frame_info.animation_dt * 30.f;
        rect->x1 = rect->x0 + cursor_size_x;
        rect->y1 = rect->y0 + cursor_size_y;
    }
    
    if(target.y0 > last_rect->y0)
    {
        if(rect->y0 < last_rect->y0)
        {
            rect->y0 = last_rect->y0;
        }
    }
    else
    {
        if(rect->y1 > last_rect->y1)
        {
            rect->y1 = last_rect->y1;
        }
    }
    
}

internal void
KaitshDrawCursorMarkHighlight(Application_Links *app, View_ID view_id, b32 is_active_view,
                              Buffer_ID buffer, Text_Layout_ID text_layout_id,
                              f32 roundness, f32 outline_thickness, Frame_Info frame_info)
{
    b32 has_highlight_range = draw_highlight_range(app, view_id, buffer, text_layout_id, roundness);
    if (!has_highlight_range){
        i32 cursor_sub_id = default_cursor_sub_id();
        
        i64 cursor_pos = view_get_cursor_pos(app, view_id);
        i64 mark_pos = view_get_mark_pos(app, view_id);
        
        if (is_active_view)
        {
            local_persist Rect_f32 rect = {0};
            Rect_f32 last_rect = rect;
            
            Rect_f32 view_rect = view_get_screen_rect(app, view_id);
            Rect_f32 target_rect = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
            Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
            
            if(cursor_pos < visible_range.start || cursor_pos > visible_range.end)
            {
                f32 width = target_rect.x1 - target_rect.x0;
                target_rect.x0 = view_rect.x0;
                target_rect.x1 = target_rect.x0 + width;
            }
            
            FleuryDoTheCursorInterpolation(app, frame_info, &rect,
                                           &last_rect, target_rect);
            
            if(global_keyboard_macro_is_recording)
            {
                draw_rectangle_fcolor(app, rect, roundness, fcolor_id(defcolor_pop2));
            }
            else
            {
                draw_rectangle_fcolor(app, rect, roundness, fcolor_id(defcolor_cursor));
            }
            //draw_character_block(app, text_layout_id, cursor_pos, roundness, fcolor_id(defcolor_cursor, cursor_sub_id));
            paint_text_color_pos(app, text_layout_id, cursor_pos,
                                 fcolor_id(defcolor_at_cursor));
            draw_character_wire_frame(app, text_layout_id, mark_pos,
                                      roundness, outline_thickness,
                                      fcolor_id(defcolor_mark));
        }
        else
        {
            draw_character_wire_frame(app, text_layout_id, mark_pos,
                                      roundness, outline_thickness,
                                      fcolor_id(defcolor_mark));
            draw_character_wire_frame(app, text_layout_id, cursor_pos,
                                      roundness, outline_thickness,
                                      fcolor_id(defcolor_cursor, cursor_sub_id));
        }
    }
}