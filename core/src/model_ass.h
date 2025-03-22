class Model_ass {
    public:
        Model_ass(char *path) {
            load_model(path);
        }
        void draw(Shader &shader);	
    private:
        // model data
        vector<Mesh> meshes;
        string directory;

        void load_model(string path);
        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, 
                                             string typeName);
};