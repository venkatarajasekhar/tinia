#pragma once
#include <tinia/renderlist/RenderList.hpp>
#include <tinia/renderlist/Item.hpp>
#include <tinia/renderlist/DataBase.hpp>

namespace tinia {
namespace renderlist {

/** Object to hold the source code for a shader program. */
class Shader : public Item
{
    friend class DataBase;
public:

    /** Get vertex shader stage source code. */
    const std::string&
    vertexStage() const { return m_vertex; }

    /** Set vertex shader stage source code. */
    Shader*
    setVertexStage( const std::string& src )
    { m_vertex = src; m_db.taint( this, true ); return this; }

    /** Get tessellation control stage source code. */
    const std::string&
    tessCtrlStage() const { return m_tess_ctrl; }

    /** Set tessellation control stage source code. */
    Shader*
    setTessCtrlStage( const std::string& src )
    { m_tess_ctrl = src; m_db.taint( this, true ); return this; }

    /** Get tessellation evaluation stage source code. */
    const std::string&
    tessEvalStage() const { return m_tess_eval; }

    /** Set tessellation evaluation stage source code. */
    Shader*
    setTessEvalStage( const std::string& src )
    { m_tess_eval = src; m_db.taint( this, true ); return this; }

    /** Get geometry shader stage source code. */
    const std::string&
    geometryStage() const { return m_geometry; }

    /** Set geometry shader stage source code. */
    Shader*
    setGeometryStage( const std::string& src )
    { m_geometry = src; m_db.taint( this, true ); return this;  }

    /** Get fragment stage source code. */
    const std::string&
    fragmentStage() const { return m_fragment; }

    /** Set fragment stage source code. */
    Shader*
    setFragmentStage( const std::string& src )
    { m_fragment = src; m_db.taint( this, true ); return this; }


protected:
    std::string m_vertex;
    std::string m_tess_ctrl;
    std::string m_tess_eval;
    std::string m_geometry;
    std::string m_fragment;

    Shader( Id id, DataBase& db, const std::string& name )
        : Item( id, db, name )
    {}

};


} // of namespace renderlist
} // of namespace tinia
