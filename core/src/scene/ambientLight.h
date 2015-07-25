#pragma once

#include "light.h"

namespace Tangram {

class AmbientLight : public Light {
public:
    
    AmbientLight(const std::string& _name, bool _dynamic = false);
    virtual ~AmbientLight();
    
    virtual void setupProgram(const std::shared_ptr<View>& _view, std::shared_ptr<ShaderProgram> _program) override;
    
protected:

    /*  GLSL block code with structs and need functions for this light type */
    virtual std::string getClassBlock() override;
    virtual std::string getInstanceDefinesBlock() override;
    virtual std::string getInstanceAssignBlock() override;
    virtual const std::string& getTypeName() override;

    static std::string s_classBlock;

private:

    static std::string s_typeName;
    
};

}
