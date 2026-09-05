# JSRF Native checkpoint 03

Local source commit: `2f3ce5cea9bf1bb6455c72a4ab71f4c77b20b752`

Source archive SHA-256: `aa5619a887143ad6f660bf5e3a8e6c453718cb9483637d1e923cd6ef1ce76f35`

Fresh verification with all four retail Media archives extracted: 18/18 CTest tests passing.

New in this checkpoint:
- Retail Garage indexed stage visual parser for `stg00_00.dat` through `stg00_04.dat`.
- 21 visual models, 7 stage MDLB entries, 76 unique indexed textures.
- 55,435 decoded Garage visual vertices and 32,383 triangles.
- Material/render group reconstruction, including independent stripped-material spans.
- DXT1 and DXT3 retail stage texture decoding.
- Native scene renderer now draws the real textured Garage visual geometry instead of the checker ground when the Garage stage is supplied.
- Windows/X11 window and screenshot paths load the retail Garage visual stage.

Current limitation: the interactive movement loop is still the old placeholder movement and has not yet been promoted as authentic gameplay. Collision/locomotion integration is the next target.
