pause = 1				-- wait for a key stroke at the end of the process

batch_file = 0			-- write batch file for quick redecompilation and copies BSP file
statistics = 0			-- writes statistics text file

export_raw_lumps = 0	-- exports raw lumps (lump splitter)
export_html = 0			-- exports lumps / bsp in HTML format, else in TXT format
export_bsp = 0			-- exports interpreted data of all lumps
export_lumps = 0		-- exports interpreted data per lump

map = 1					-- writes MAP file
map_version = 5			-- MAP file format for either (CoD) 1, 2, 4 or 5; 0 defaults to BSP version
map_layers = 1			-- enables layers
map_brushes = 1 		-- appends brushes
map_triangles = 1		-- appends all triangles stored as in the draw lumps
map_entities = 1		-- appends entities
map_models = 1			-- appends models
map_spawns = 1			-- appends spawns

caulk_brushes = 0		-- makes all brushes caulk (except tool textures)
caulk_tool_brushes = 0	-- makes all tool brushes caulk
caulk_triangles = 1		-- makes all triangles caulk (except tool textures)

layer_split = 1