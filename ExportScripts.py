import bpy
import mathutils
from math import *

class Transform():
    def __init__(self,obj):
        self.pos = obj.location
        self.rot = obj.rotation_euler
        self.scale = obj.scale
        self.parent = obj.parent
    
    def toC(self):
        return ("Transform(" +
        vectorToC(self.pos) + ", " +
        eulerToC(self.rot) + ", " + 
        vectorToC(self.scale,s=1) + ", " + 
        ")")
    
    def __repr__(self):
        return self.toC()

class StaticColliderDef:
    def __init__(self,collider):
        self.transform = Transform(collider)
        if(collider.type == "EMPTY"):
            self.tpe = collider.empty_display_type
            self.meshRef = None
        elif(collider.type == "MESH"):
            self.tpe = "MESH"
            self.meshRef = collider.data.name
    
    def toC(self):
        if(self.tpe == "CUBE"):
            return ("agg.addStaticBox(" +
            vectorToC(self.transform.pos,cl="PxTransform") + ", " +
            vectorToC(self.transform.scale,cl="PxBoxGeometry") + ");"
            )
        else:
            return ""
    
    def __repr__(self):
        return self.toC()

class MaterialDef():
    materials = dict()
    
    def __init__(self,material):
        self.name = material if isinstance(material,str) else material.name
    
    def get(material):
        materials = MaterialDef.materials
        key = material if isinstance(material,str) else material.name
        if not key in materials:
            materials[key] = MaterialDef(material)
        return materials[key]
    
    def toC(self):
        return ""
    
    def toCGet(self):
        return "Resources.GetMaterial(" + stc(self.name) + ")"
    
    def __repr__(self):
        return self.toC()
missingMaterial = MaterialDef("missing")

class MeshDef():
    meshes = dict()
    
    def get(mesh):
        meshes = MeshDef.meshes
        key = mesh.name
        if not key in meshes:
            meshes[key] = MeshDef(mesh)
        return meshes[key]
    
    def __init__(self,data):
        self.name = data.name
        self.meshRef = data
        self.material = missingMaterial if (len(data.materials) == 0) else (MaterialDef.get(data.materials[0]))
    
    def toC(self):
        return ""
    
    def toCGet(self):
        return "Resources.GetMesh(" + stc(self.name) + ")"
    
    def __repr__(self):
        return self.toC()

class GameObjectDef():
    def __init__(self,obj):
        self.name = obj.name
        self.transform = Transform(obj)
        self.mesh = MeshDef.get(obj.data)

    def toC(self):
        return ("scene.AddObject(std::make_unique<GameObject>(" + 
        self.transform.toC() + ", " +
        self.mesh.toCGet() + ", " +
        self.mesh.material.toCGet() + ", \"" +
        stc(self.name) +
        "\"));"
        )
    
    def __repr__(self):
        return self.toC()

class LightDef():
    def __init__(self,obj):
        self.pos = obj.location
        self.lightData = obj.data
        self.type = obj.data.type
        self.color = obj.data.energy * obj.data.color
        self.direction = mathutils.Vector((0.0, 0.0, -1.0))
        self.direction.rotate(obj.rotation_euler)
    
    def toC(self):
        if self.type == "POINT":
            return ("PointLight(" + vectorToC(self.pos) + ", " + 
            colortoC(self.color) + #", " + 
            #calculate Attenuation parameters
            ");"
            )
        elif self.type == "SUN":
            return ("DirectionalLight(" + vectorToC(self.direction) + ", " + 
            colortoC(self.color) + 
            ");"
            )
        elif self.type == "SPOT":
            coneOuter = sin(self.lightData.spot_size)
            coneInner = coneOuter*self.lightData.spot_blend #isn't how blender does it
            return ("SpotLight(" + vectorToC(self.pos) + ", " + 
            vectorToC(self.direction) + ", " + 
            "glm::vec2(" + ftc(round(coneInner,3)) + ", " + ftc(round(coneOuter,3)) + "), " + #fix spotlight radius
            colortoC(self.color) + #", " + 
            # + ", " + calculate Attenuation parameters +
            ");"
            )
    
    def __repr__(self):
        return self.toC()

#float to c
def ftc(f):
    return str(f) + "f"

#string to c
def stc(s):
    return "\"" + s + "\""

def colortoC(color, decim = 2):
    t = (round(color.r,decim),round(color.g,decim),round(color.b,decim))
    return "glm::vec3(" + ftc(t[0]) + "," + ftc(t[1]) + "," + ftc(t[2]) + ")"

def vectorToC(vector, decim = 3, cl = "glm::vec3", s = -1):
    if(vector.length_squared==0):
        return cl + "()"
    t = vector.to_tuple(decim)
    return cl + "(" + ftc(t[0]) + "," + ftc(t[2]) + "," + ftc(s * t[1]) + ")"

def eulerToC(euler, decim = 5):
    if(euler.x==0 and euler.y == 0 and euler.z == 0):
        return "glm::vec3" + "()"
    t = (round(euler.x,decim),round(euler.y,decim),round(euler.z,decim))
    return "(" + ftc(t[0]) + "," + ftc(t[1]) + "," + ftc(t[2]) + ")"

def ScopeLines(table, newline = True):
    nl = "\n" if newline else ""
    ret = "{" + nl
    ret+= table[0].toC()
    for entry in table[1:]:
        ret+= nl + entry.toC()
    ret+=nl+"}"
    return ret

def ExportStaticColliders():
    staticColliders = bpy.data.collections["Colliders"].children["Static"].objects

    ColDefs = []
    for collider in staticColliders:
        ColDefs.append(StaticColliderDef(collider))

    return "//StaticColliders\n" + ScopeLines(ColDefs)

def ExportVisualStaticDefs():
    objects = bpy.data.collections["VisualStatic"].objects

    ObjDefs = []
    for obj in objects:
        ObjDefs.append(GameObjectDef(obj))

    return "//StaticMeshes\n" + ScopeLines(ObjDefs)

def ExportGameObjectDefs():
    objects = bpy.data.collections["Dynamic"].objects

    ObjDefs = []
    for obj in objects:
        ObjDefs.append(GameObjectDef(obj))

    return "//GameObjects\n" + ScopeLines(ObjDefs)

def ExportStaticLights():
    lights = bpy.data.collections["StaticLights"].objects

    LightDefs = []
    for light in lights:
        LightDefs.append(LightDef(light))

    return "//StaticLights\n" + ScopeLines(LightDefs)

def CheckFolder(file):
    import os
    os.makedirs(os.path.dirname(file), exist_ok=True)

def ExportSelectedObjects(fp):
    bpy.ops.export_scene.obj(filepath=fp, check_existing = True,use_selection=True,use_edges=False,use_materials=False,use_triangles=True,use_blen_objects=False)

def ExportMeshes(meshes, folder=""):
    if(len(meshes)==0): return
    folder = basePath + folder
    CheckFolder(folder)
    bpy.ops.object.select_all(action="DESELECT")
    obj = bpy.data.objects.new("export_dummy_123456789", next(iter(meshes.values())).meshRef);
    bpy.context.collection.objects.link(obj)
    obj.select_set(True)
    
    for mesh in meshes.values():
        obj.data = mesh.meshRef
        ExportSelectedObjects(folder + mesh.name + ".obj")
    bpy.ops.object.delete()

def SelectCollection(coll):
    for obj in bpy.data.collections[coll].all_objects:
        obj.select_set(True)

def ExportDefs():
    print(ExportStaticColliders())
    print(ExportVisualStaticDefs())
    print(ExportStaticLights())
    
    if True:
        print("Writing Defs ... ", end ='')
        CheckFolder(basePath)
        f = open(basePath + "defs.txt","w")
        f.write(ExportStaticColliders() + "\n")
        f.write(ExportVisualStaticDefs() + "\n")
        f.write(ExportStaticLights() + "\n")
        f.close()
        print("done")

def Export():
    bpy.ops.object.select_all(action="DESELECT")
    SelectCollection("VisualStatic") 
    CheckFolder(basePath)
    ExportSelectedObjects(basePath + "GameScene.obj")

basePath = bpy.path.abspath("//exports\\")
#Export()
ExportDefs()
#ExportMeshes(MeshDef.meshes, "models\\")