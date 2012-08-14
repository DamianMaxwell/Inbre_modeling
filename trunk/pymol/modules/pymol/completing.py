import cmd

def get_auto_arg_list(self_cmd=cmd):
    return [
# 1st
        {
        'align'          : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'alter'          : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'bond'           : [ self_cmd.selection_sc           , 'selection'       , ''   ],        
        'as'             : [ self_cmd.repres_sc              , 'representation'  , ', ' ],
        'bg_color'       : [ lambda c=self_cmd:c._get_color_sc(c), 'color'       , ''   ],      
        'button'         : [ self_cmd.controlling.button_sc  , 'button'          , ', ' ],
        'cartoon'        : [ self_cmd.viewing.cartoon_sc     , 'cartoon'         , ', ' ],
        'cache'          : [ self_cmd.exporting.cache_action_sc , 'cache mode'   , ', ' ],
        'center'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'cealign'        : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'color'          : [ lambda c=self_cmd:c._get_color_sc(c), 'color'       , ', ' ],
        'config_mouse'   : [ self_cmd.controlling.ring_dict_sc, 'mouse cycle'    , ''   ],
        'clean'          : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'clip'           : [ self_cmd.viewing.clip_action_sc , 'clipping action' , ', ' ],
        'count_atoms'    : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'delete'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'deprotect'      : [ self_cmd.selection_sc           , 'selection'       , ''   ],        
        'disable'        : [ self_cmd.object_sc              , 'object'          , ' '  ],
        'distance'       : [ self_cmd.object_sc              , 'object'          , ''   ],
        'dss'            : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'enable'         : [ self_cmd.object_sc              , 'object'          , ' '  ],        
        'extract'        : [ self_cmd.object_sc              , 'object'          , ''   ],      
        'feedback'       : [ self_cmd.fb_action_sc           , 'action'          , ', ' ],
        'fit'            : [ self_cmd.selection_sc           , 'selection'       , ''   ],        
        'flag'           : [ self_cmd.editing.flag_sc        , 'flag'            , ', ' ],
        'full_screen'    : [ self_cmd.toggle_sc              , 'option'          , ''   ],
        'get'            : [ self_cmd.setting.setting_sc     , 'setting'         , ', ' ],
        'gradient'       : [ self_cmd.object_sc              , 'gradient'        , ', ' ],
        'group'          : [ self_cmd.group_sc               , 'group object'    , ', ' ],
        'help'           : [ self_cmd.help_sc                , 'selection'       , ''   ],
        'hide'           : [ self_cmd.repres_sc              , 'representation'  , ', ' ],
        'isolevel'       : [ self_cmd.contour_sc             , 'contour'         , ', ' ],
        'iterate'        : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'iterate_state'  : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'indicate'       : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'intra_fit'      : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'label'          : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'map_set'        : [ self_cmd.map_sc                 , 'map'             , ''   ],
        'mask'           : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'mview'          : [ self_cmd.moving.mview_action_sc , 'action'          , ''   ],
        'map_double'     : [ self_cmd.map_sc                 , 'map object'      , ', ' ],
        'map_halve'      : [ self_cmd.map_sc                 , 'map object'      , ', ' ],            
        'map_trim'       : [ self_cmd.map_trim               , 'map object'      , ', ' ],
        'matrix_copy'    : [ self_cmd.object_sc              , 'object'          , ', ' ],            
        'matrix_reset'   : [ self_cmd.object_sc              , 'object'          , ', ' ],            
        'order'          : [ self_cmd.selection_sc           , 'name'            , ''   ],
        'origin'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'pair_fit'       : [ self_cmd.selection_sc           , 'selection'       , ', ' ],        
        'protect'        : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'pseudoatom'     : [ self_cmd.object_sc              , 'object'          , ''   ],            
        'ramp_new'       : [ self_cmd.object_sc              , 'ramp'            , ', ' ],
        'reference'      : [ self_cmd.editing.ref_action_sc  , 'action'          , ', ' ],
        'remove'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'reinitialize'   : [ self_cmd.commanding.reinit_sc   , 'option'          , ''   ],
        'scene'          : [ self_cmd._pymol._scene_dict_sc  , 'scene'           , ''   ],
        'sculpt_activate': [ self_cmd.object_sc              , 'object'          , ''   ],
        'set'            : [ self_cmd.setting.setting_sc     , 'setting'         , ', ' ],
        'set_bond'       : [ self_cmd.setting.setting_sc     , 'setting'         , ', ' ],            
        'set_name'       : [ self_cmd.selection_sc     ,       'name'            , ', ' ],
        'show'           : [ self_cmd.repres_sc              , 'representation'  , ', ' ],
        'smooth'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'space'          : [ self_cmd.space_sc               , 'space'           , ''   ],      
        'split_states'   : [ self_cmd.object_sc              , 'object'          , ', ' ],
        'super'          : [ self_cmd.selection_sc           , 'selection'       , ', ' ],        
        'stereo'         : [ self_cmd.stereo_sc              , 'option'          , ''   ],      
        'symmetry_copy'  : [ self_cmd.object_sc              , 'object'          , ', ' ],            
        'unmask'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'unset'          : [ self_cmd.setting.setting_sc     , 'setting'         , ', ' ],
        'unset_bond'     : [ self_cmd.setting.setting_sc     , 'setting'         , ', ' ],
        'update'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'valence'        : [ self_cmd.editing.order_sc       , 'order'           , ', ' ],
        'view'           : [ self_cmd._pymol._view_dict_sc   , 'view'            , ''   ],         
        'window'         : [ self_cmd.window_sc              , 'action'          , ', ' ],      
        'zoom'           : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        },
# 2nd
        {
        'align'          : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'as'             : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'bond'           : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'button'         : [ self_cmd.controlling.but_mod_sc , 'modifier'        , ', ' ],
        'cache'          : [ self_cmd._pymol._scene_dict_sc  , 'scene'           , ''   ],
        'cealign'        : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'color'          : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'create'         : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'distance'       : [ self_cmd.selection_sc           , 'selection'       , ''   ],            
        'extract'        : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'feedback'       : [ self_cmd.fb_module_sc           , 'module'          , ', ' ],
        'flag'           : [ self_cmd.selection_sc           , 'selection'       , ', ' ],   
        'get'            : [ self_cmd.object_sc              , 'object'          , ', ' ],
        'gradient'       : [ self_cmd.map_sc                 , 'map object'      , ', ' ],
        'group'          : [ self_cmd.object_sc              , 'object'          , ', ' ],
        'hide'           : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'isomesh'        : [ self_cmd.map_sc                 , 'map object'      , ', ' ],
        'isosurface'     : [ self_cmd.map_sc                 , 'map object'      , ', ' ],
        'volume'         : [ self_cmd.map_sc                 , 'map object'      , ', ' ],
        'select'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'save'           : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'load'           : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'load_traj'      : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'map_set'        : [ self_cmd.editing.map_op_sc      , 'operator'        , ', ' ],
        'map_new'        : [ self_cmd.creating.map_type_sc   , 'map type'        , ', ' ],
        'map_trim'       : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'matrix_copy'    : [ self_cmd.object_sc              , 'object'          , ', ' ],
        'order'          : [ self_cmd.boolean_sc             , 'sort'            , ', ' ],
        'pair_fit'       : [ self_cmd.selection_sc           , 'selection'       , ', ' ],                
        'reference'      : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'scene'          : [ self_cmd.viewing.scene_action_sc, 'scene action'    , ', ' ],
        'set_name'       : [ self_cmd.selection_sc     ,       'name'            , ''   ],
        'show'           : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'slice_new'      : [ self_cmd.map_sc                 , 'map object'      , ', ' ],
        'spectrum'       : [ self_cmd.palette_sc             , 'palette'         , ''   ],      
        'super'          : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'symexp'         : [ self_cmd.object_sc              , 'object'          , ', ' ],   
        'symmetry_copy'  : [ self_cmd.object_sc              , 'object'          , ', ' ],            
        'view'           : [ self_cmd.viewing.view_sc        , 'view action'     , ''   ],
        'unset'          : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'unset_bond'     : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'update'         : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'ramp_new'       : [ self_cmd.map_sc                 , 'map object'      , ', ' ],      
        'valence'        : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        },
#3rd
        {
        'button'         : [ self_cmd.controlling.but_act_sc , 'button action'   , ''   ],
        'distance'       : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'feedback'       : [ self_cmd.fb_mask_sc             , 'mask'            , ''   ],            
        'flag'           : [ self_cmd.editing.flag_action_sc , 'flag action'     , ''   ],
        'group'          : [ self_cmd.creating.group_action_sc, 'group action'    , ''   ],
        'map_set'        : [ self_cmd.map_sc                 , 'map'             , ' '  ],
        'order'          : [ self_cmd.controlling.location_sc, 'location'        , ', ' ],
        'set'            : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'set_bond'       : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'spectrum'       : [ self_cmd.selection_sc           , 'selection'       , ''   ],
        'symexp'         : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'unset_bond'     : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'valence'        : [ self_cmd.selection_sc           , 'selection'       , ', ' ],                
        },
#4th
        {
        'ramp_new'       : [ self_cmd.creating.ramp_spectrum_sc , 'ramp color spectrum' , ', ' ],      
        'map_new'        : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'isosurface'     : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'volume'         : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'isomesh'        : [ self_cmd.selection_sc           , 'selection'       , ', ' ],
        'set_bond'       : [ self_cmd.selection_sc            , 'selection'      , ', ' ],            
        'valence'        : [ self_cmd.selection_sc           , 'selection'       , ', '  ],                
        }
        ]
