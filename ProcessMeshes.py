bl_info = {
    "name": "Asset Processor",
    "blender": (4, 1, 0),
    "category": "Object",
    "version": (1, 0, 12),
    "location": "View3D > N-panel > Asset Processor",
    "description": "Addon to import and process multiple objects into the Blender asset library.",
    "author": "Your Name",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "support": "COMMUNITY",
}

import bpy
import os
from bpy_extras.io_utils import ImportHelper

def setup_lighting():
    # Delete all existing lights in the scene
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.object.select_by_type(type='LIGHT')
    bpy.ops.object.delete()

    # Create a new light
    bpy.ops.object.light_add(type='SUN')
    light = bpy.context.object
    light.data.energy = 3

def setup_camera(obj):
    # Delete all existing cameras in the scene
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.object.select_by_type(type='CAMERA')
    bpy.ops.object.delete()

    # Create a new camera
    bpy.ops.object.camera_add()
    camera = bpy.context.object
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = max(obj.dimensions)

    # Position the camera to view the object
    camera.location = obj.location
    camera.location.z += 1
    camera.rotation_euler = (0.785398, 0, 0.785398)

class ImportAndProcessOperator(bpy.types.Operator, ImportHelper):
    """Operator to import and process multiple objects"""
    bl_idname = "object.import_and_process"
    bl_label = "Import and Process Objects"
    
    # ImportHelper mixin class uses this
    filename_ext = ".obj;.fbx"

    filter_glob: bpy.props.StringProperty(
        default="*.obj;*.fbx",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        # Import the objects
        bpy.ops.import_scene.obj(filepath=self.filepath)
        
        # Get the directory from the addon preferences
        prefs = context.preferences.addons[__name__].preferences
        dir_path = bpy.path.abspath(prefs.asset_library_path)

        # Get the category name from the scene properties
        category_name = context.scene.asset_processor_category_name

        # Process the imported objects
        for obj in bpy.context.selected_objects:
            if obj.type == 'MESH':
                setup_lighting()
                setup_camera(obj)
                bpy.ops.object.select_all(action='DESELECT')
                obj.select_set(True)  # Select the object
                bpy.context.view_layer.objects.active = obj  # Set the object as the active object
                bpy.ops.asset.mark()

                # Create a filepath for the asset
                asset_dir = os.path.join(dir_path, category_name)
                os.makedirs(asset_dir, exist_ok=True)
                file_path = os.path.join(asset_dir, obj.name + ".blend")

                # Save the asset in a .blend file
                bpy.data.libraries.write(file_path, set([obj]), fake_user=True)
        
        return {'FINISHED'}

class ProcessSelectedOperator(bpy.types.Operator):
    """Operator to process selected objects"""
    bl_idname = "object.process_selected"
    bl_label = "Process Selected Objects"

    def execute(self, context):
        # Get the directory from the addon preferences
        prefs = context.preferences.addons[__name__].preferences
        dir_path = bpy.path.abspath(prefs.asset_library_path)

        # Get the category name from the scene properties
        category_name = context.scene.asset_processor_category_name

        # Process the selected objects
        for obj in bpy.context.selected_objects:
            if obj.type == 'MESH':
                setup_lighting()
                setup_camera(obj)
                bpy.ops.object.select_all(action='DESELECT')
                obj.select_set(True)  # Select the object
                bpy.context.view_layer.objects.active = obj  # Set the object as the active object
                bpy.ops.asset.mark()

                # Create a filepath for the asset
                asset_dir = os.path.join(dir_path, category_name)
                os.makedirs(asset_dir, exist_ok=True)
                file_path = os.path.join(asset_dir, obj.name + ".blend")

                # Save the asset in a .blend file
                bpy.data.libraries.write(file_path, set([obj]), fake_user=True)
        
        return {'FINISHED'}

class AssetProcessorPanel(bpy.types.Panel):
    """Panel for the Asset Processor addon"""
    bl_label = "Asset Processor"
    bl_idname = "OBJECT_PT_asset_processor"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Asset Processor"

    def draw(self, context):
        layout = self.layout

        layout.prop(context.scene, "asset_processor_category_name")
        layout.operator("object.import_and_process", text="Import and Process Objects")
        layout.operator("object.process_selected", text="Process Selected Objects")

class AssetProcessorPreferences(bpy.types.AddonPreferences):
    """Preferences for the Asset Processor addon"""
    bl_idname = __name__

    asset_library_path: bpy.props.StringProperty(
        name="Asset Library Path",
        subtype='DIR_PATH',
    )

    def draw(self, context):
        layout = self.layout
        layout.prop(self, "asset_library_path")

def register():
    bpy.utils.register_class(ImportAndProcessOperator)
    bpy.utils.register_class(ProcessSelectedOperator)
    bpy.utils.register_class(AssetProcessorPanel)
    bpy.utils.register_class(AssetProcessorPreferences)
    bpy.types.Scene.asset_processor_category_name = bpy.props.StringProperty(name="Category Name", default="My Assets")

def unregister():
    bpy.utils.unregister_class(ImportAndProcessOperator)
    bpy.utils.unregister_class(ProcessSelectedOperator)
    bpy.utils.unregister_class(AssetProcessorPanel)
    bpy.utils.unregister_class(AssetProcessorPreferences)
    del bpy.types.Scene.asset_processor_category_name

if __name__ == "__main__":
    register()
