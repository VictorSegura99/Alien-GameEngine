#include "ModuleImporter.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "Model3D.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilInit();
	iluInit();
	ilutInit();

	return true;
}

bool ModuleImporter::Start()
{
	App->objects->base_game_object = new GameObject();
	App->objects->base_game_object->AddComponent(new ComponentTransform());
	
	//LoadTextureFile("Assets/Textures/Baker.dds");
	LoadModelFile("Assets/Models/BakerHouse.fbx");
	return true;
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();

	return true;
}

bool ModuleImporter::LoadModelFile(const char* path)
{
	bool ret = true;

	LOG("Loading %s", path);
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr) {
		InitScene(scene, path);
		LOG("Succesfully loaded");
	}
	else {
		ret = false;
		LOG("Error loading model %s", path);
		LOG("Error type: %s", aiGetErrorString());
	}
	aiReleaseImport(scene);
	return ret;
}

void ModuleImporter::InitScene(const aiScene* scene, const char* path)
{
	//data->textures.resize(scene->mNumMaterials); ////////////////
	/*if (scene->HasMeshes()) {
		for (uint i = 0; i < scene->mNumMeshes; ++i) {
			InitMesh(scene->mMeshes[i], path);
		}
	}*/
	// TODO: load .obj and other 3Ds that have textures
	/*if (scene->HasTextures()) {
		for (uint i = 0; i < scene->mNumTextures; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];
			data->meshes.push_back(InitMesh(mesh));
		}
	}*/

	// create the parent of the all fbx/obj...
	parent_object = new GameObject();
	parent_object->AddComponent(new ComponentTransform());
	// set it's parent to the "invisible" game object
	parent_object->parent = App->objects->base_game_object;
	App->objects->base_game_object->AddChild(parent_object);
	// set parent name, we must change that
	parent_object->SetName("FBX Parent, Change this");
	
	// start recursive function to pass through all nodes
	LoadSceneNode(scene->mRootNode, scene, parent_object);

	parent_object = nullptr;
}

void ModuleImporter::LoadSceneNode(const aiNode* node, const aiScene* scene, GameObject* parent)
{
	GameObject* next_parent = nullptr;
	for (uint i = 0; i < node->mNumMeshes; ++i) {
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		next_parent = LoadNodeMesh(scene, node, mesh, parent);
	}
	
	for (uint i = 0; i < node->mNumChildren; ++i) {
		LoadSceneNode(node->mChildren[i], scene, next_parent);
	}
}

GameObject* ModuleImporter::LoadNodeMesh(const aiScene * scene, const aiNode* node, const aiMesh* ai_mesh, GameObject* parent)
{
	// get local transformations
	ComponentTransform* transform = new ComponentTransform();

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	transform->local_position = pos;
	transform->local_scale = scale;
	transform->local_rotation = rot;
	transform->complete_transformation.FromTRS(pos, rot, scale);

	// get mesh data
	ComponentMesh* mesh = new ComponentMesh();

	// get vertex
	mesh->vertex = new float[ai_mesh->mNumVertices * 3];
	memcpy(mesh->vertex, ai_mesh->mVertices, sizeof(float) * ai_mesh->mNumVertices * 3);
	mesh->num_vertex = ai_mesh->mNumVertices;
	// get index
	if (ai_mesh->HasFaces())
	{
		mesh->num_index = ai_mesh->mNumFaces * 3;
		mesh->index = new uint[mesh->num_index]; // assume each face is a triangle
		for (uint i = 0; i < ai_mesh->mNumFaces; ++i)
		{
			if (ai_mesh->mFaces[i].mNumIndices != 3) {
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else {
				memcpy(&mesh->index[i * 3], ai_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}
	// get normals
	if (ai_mesh->HasNormals())
	{
		mesh->normals = new float[ai_mesh->mNumVertices * 3];
		memcpy(mesh->normals, ai_mesh->mNormals, sizeof(float) * ai_mesh->mNumVertices * 3);

		mesh->center_point_normal = new float[ai_mesh->mNumFaces * 3];
		mesh->center_point = new float[ai_mesh->mNumFaces * 3];
		mesh->num_faces = ai_mesh->mNumFaces;
		for (uint i = 0; i < mesh->num_index; i += 3)
		{
			uint index1 = mesh->index[i] * 3;
			uint index2 = mesh->index[i + 1] * 3;
			uint index3 = mesh->index[i + 2] * 3;

			vec3 x0(mesh->vertex[index1], mesh->vertex[index1 + 1], mesh->vertex[index1 + 2]);
			vec3 x1(mesh->vertex[index2], mesh->vertex[index2 + 1], mesh->vertex[index2 + 2]);
			vec3 x2(mesh->vertex[index3], mesh->vertex[index3 + 1], mesh->vertex[index3 + 2]);

			vec3 v0 = x0 - x2;
			vec3 v1 = x1 - x2;
			vec3 n = cross(v0, v1);

			vec3 normalized = normalize(n);

			mesh->center_point[i] = (x0.x + x1.x + x2.x) / 3;
			mesh->center_point[i + 1] = (x0.y + x1.y + x2.y) / 3;
			mesh->center_point[i + 2] = (x0.z + x1.z + x2.z) / 3;

			mesh->center_point_normal[i] = normalized.x;
			mesh->center_point_normal[i + 1] = normalized.y;
			mesh->center_point_normal[i + 2] = normalized.z;
		}
	}
	// get UV
	if (ai_mesh->HasTextureCoords(0)) {
		mesh->uv_cords = new float[ai_mesh->mNumVertices * 3];
		memcpy(mesh->uv_cords, (float*)ai_mesh->mTextureCoords[0], sizeof(float) * ai_mesh->mNumVertices * 3);

		// TODO LOAD TEXTURES & MATERIALS

	}

	ComponentMaterial* material = new ComponentMaterial();

	/*aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];
	uint numTextures = ai_material->GetTextureCount(aiTextureType_DIFFUSE);
	aiString path;
	ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &path);*/


	InitMeshBuffers(mesh);


	////TEST
	//ComponentMaterial* material = new ComponentMaterial();
	//material->id_texture = test_id;


	GameObject* ret = new GameObject();
	if (parent == nullptr) { // the first nodes that ara child of the root node would not have a parent, so we put that it's parent is the fbx parent we created before		
		ret->parent = parent_object;
		parent_object->AddChild(ret);
	}
	else {
		ret->parent = parent;
		parent->AddChild(ret);
	}
	ret->AddComponent(transform);
	ret->AddComponent(mesh);
	ret->AddComponent(material);
	ret->SetName(node->mName.C_Str());


	return ret;
}

void ModuleImporter::InitMeshBuffers(ComponentMesh* mesh)
{
	// vertex
	glGenBuffers(1, &mesh->id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3,
		&mesh->vertex[0], GL_STATIC_DRAW);

	// index
	glGenBuffers(1, &mesh->id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_index,
		&mesh->index[0], GL_STATIC_DRAW);

	// UV
	glGenBuffers(1, &mesh->id_uv);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_uv);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3,
		&mesh->uv_cords[0], GL_STATIC_DRAW);

	// normals
	glGenBuffers(1, &mesh->id_normals);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3,
		&mesh->normals[0], GL_STATIC_DRAW);
}

//void ModuleImporter::InitMesh(const aiMesh* ai_mesh, const char* path)
//{
//	Model3D* data = new Model3D();
//	data->path = path;
//	data->material_index = ai_mesh->mMaterialIndex;
//
//	const aiVector3D zeros(0.0f, 0.0f, 0.0f);
//	data->vertex = new float[ai_mesh->mNumVertices * 3];
//	memcpy(data->vertex, ai_mesh->mVertices, sizeof(float) * ai_mesh->mNumVertices * 3);
//	data->num_vertex = ai_mesh->mNumVertices;
//
//	if (ai_mesh->HasFaces())
//	{
//		data->num_index = ai_mesh->mNumFaces * 3;
//		data->index = new uint[data->num_index]; // assume each face is a triangle
//		for (uint i = 0; i < ai_mesh->mNumFaces; ++i)
//		{
//			if (ai_mesh->mFaces[i].mNumIndices != 3) {
//				LOG("WARNING, geometry face with != 3 indices!");
//			}
//			else {
//				memcpy(&data->index[i * 3], ai_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
//			}
//		}
//	}
//	if (ai_mesh->HasNormals())
//	{
//		data->normals = new float[ai_mesh->mNumVertices * 3];
//		memcpy(data->normals, ai_mesh->mNormals, sizeof(float) * ai_mesh->mNumVertices * 3);
//
//		data->center_point_normal = new float[ai_mesh->mNumFaces * 3];
//		data->center_point = new float[ai_mesh->mNumFaces * 3];
//		data->num_faces = ai_mesh->mNumFaces;
//		for (uint i = 0; i < data->num_index; i += 3)
//		{
//			uint index1 = data->index[i] * 3;
//			uint index2 = data->index[i + 1] * 3;
//			uint index3 = data->index[i + 2] * 3;
//
//			vec3 x0(data->vertex[index1], data->vertex[index1 + 1], data->vertex[index1 + 2]);
//			vec3 x1(data->vertex[index2], data->vertex[index2 + 1], data->vertex[index2 + 2]);
//			vec3 x2(data->vertex[index3], data->vertex[index3 + 1], data->vertex[index3 + 2]);
//
//			vec3 v0 = x0 - x2;
//			vec3 v1 = x1 - x2;
//			vec3 n = cross(v0, v1);
//			
//			vec3 normalized = normalize(n);
//
//			data->center_point[i] = (x0.x + x1.x + x2.x) / 3;
//			data->center_point[i + 1] = (x0.y + x1.y + x2.y) / 3;
//			data->center_point[i + 2] = (x0.z + x1.z + x2.z) / 3;
//
//			data->center_point_normal[i] = normalized.x;
//			data->center_point_normal[i + 1] = normalized.y;
//			data->center_point_normal[i + 2] = normalized.z;
//		}
//	}
//	if (ai_mesh->HasTextureCoords(0)) {
//		data->uv_cords = new float[ai_mesh->mNumVertices * 3];
//		memcpy(data->uv_cords, (float*)ai_mesh->mTextureCoords[0], sizeof(float) * ai_mesh->mNumVertices * 3);
//	}
//
//	InitGLBuffers(data);
//
//	App->objects->game_objects.push_back(data);
//}
//
//void ModuleImporter::InitGLBuffers(Model3D* model3D)
//{
//	// vertex
//	glGenBuffers(1, &model3D->id_vertex);
//	glBindBuffer(GL_ARRAY_BUFFER, model3D->id_vertex);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model3D->num_vertex * 3,
//		&model3D->vertex[0], GL_STATIC_DRAW);
//
//	// index
//	glGenBuffers(1, &model3D->id_index);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model3D->id_index);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * model3D->num_index,
//		&model3D->index[0], GL_STATIC_DRAW);
//
//	// UV
//	glGenBuffers(1, &model3D->id_uv);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model3D->id_uv);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * model3D->num_vertex * 3,
//		&model3D->uv_cords[0], GL_STATIC_DRAW);
//
//	// normals
//	glGenBuffers(1, &model3D->id_normals);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model3D->id_normals);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * model3D->num_vertex * 3,
//		&model3D->normals[0], GL_STATIC_DRAW);
//
//
//}

bool ModuleImporter::LoadTextureFile(const char* path)
{
	bool ret = true;

	if (ilLoadImage(path)) {
		ILuint Width, Height;
		Width = ilGetInteger(IL_IMAGE_WIDTH);
		Height = ilGetInteger(IL_IMAGE_HEIGHT);

		ILubyte* Data = ilGetData();
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &test_id);
		glBindTexture(GL_TEXTURE_2D, test_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
	}
	
	/*std::vector<GameObject*>::iterator item = App->objects->game_objects.begin();
	for (; item != App->objects->game_objects.end(); ++item) {
		if (*item != nullptr) {
			(*item)->id_texture = test_id;
		}
	}*/

	return ret;
}