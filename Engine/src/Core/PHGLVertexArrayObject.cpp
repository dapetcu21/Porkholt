/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHThread.h>

#define MAX_VAO_ATTR 16

PHGLVertexArrayObject::PHGLVertexArrayObject(PHGameManager * gameManager) : vao(0), bound(false), gm(gameManager), elementVBO(NULL), drawType(drawNone), editBind(false)
{
    if (PHGL::glGenVertexArrays)
        PHGL::glGenVertexArrays(1, &vao);
}

PHGLVertexArrayObject::~PHGLVertexArrayObject()
{
    if (vao && PHGL::glDeleteVertexArrays)
    {
        if (PHThread::currentThread() == PHThread::mainThread())
            PHGL::glDeleteVertexArrays(1, &vao);
        else
            gm->queueDeleteVAO(vao);
    }
    for (map<GLuint, attribute*>::iterator i = attributes.begin(); i!=attributes.end(); i++)
        delete i->second;
    for (map<GLuint, PHGLVBO*>::iterator i = attributeVBOs.begin(); i!=attributeVBOs.end(); i++)
        if (i->second)
            i->second->release();
    if (elementVBO)
        elementVBO->release();
}   

#define bind_begin PHGLVAO * oldvao = gm->boundVAO(); bindToEdit();
#define bind_begin_draw PHGLVAO * oldvao = gm->boundVAO(); bind();
#define bind_end gm->bindVAO(oldvao);

void PHGLVertexArrayObject::bindToEdit()
{
    editBind = true;
    bind();
    editBind = false;
}

void PHGLVertexArrayObject::fakeBind()
{
    if (!editBind)
    {
        PHGLVertexBufferObject * vbo = gm->boundVBO(PHGLVBO::arrayBuffer);
        if (gm->useShaders())
        {
            uint32_t mask = 0;
            for (map<GLuint, attribute*>::iterator i = attributes.begin(); i!=attributes.end(); i++)
            {
                attribute * a = i->second;
                if (a->vbo)
                    a->vbo->bindTo(PHGLVBO::arrayBuffer);
                PHGL::glVertexAttribPointer(i->first, a->size, a->type, a->normalized, a->stride, (const GLvoid*)a->offset);
                mask |= (1<< (i->first));
            }
            gm->setGLAttributeStates(mask);
        } else {
            uint32_t mask = 0;
            map<GLuint, attribute*>::iterator i;
            
            i = attributes.find(PHIMAGEATTRIBUTE_POS);
            if (i!=attributes.end())
            {
                attribute * a = i->second;
                if (a->vbo)
                    a->vbo->bindTo(PHGLVBO::arrayBuffer);
                PHGL::glVertexPointer(a->size, a->type, a->stride, (const GLvoid*)a->offset);
                mask |= (1 << PHIMAGEATTRIBUTE_POS);
            }
            
            i = attributes.find(PHIMAGEATTRIBUTE_TXC);
            if (i!=attributes.end())
            {
                attribute * a = i->second;
                if (a->vbo)
                    a->vbo->bindTo(PHGLVBO::arrayBuffer);
                PHGL::glTexCoordPointer(a->size, a->type, a->stride, (const GLvoid*)a->offset);
                mask |= (1 << PHIMAGEATTRIBUTE_TXC);
            }
            
            i = attributes.find(PHIMAGEATTRIBUTE_CLR);
            if (i!=attributes.end())
            {
                attribute * a = i->second;
                if (a->vbo)
                    a->vbo->bindTo(PHGLVBO::arrayBuffer);
                PHGL::glColorPointer(a->size, a->type, a->stride, (const GLvoid*)a->offset);
                mask |= (1 << PHIMAGEATTRIBUTE_CLR);
            }
            
            i = attributes.find(PHIMAGEATTRIBUTE_NRM);
            if (i!=attributes.end())
            {
                attribute * a = i->second;
                if (a->vbo)
                    a->vbo->bindTo(PHGLVBO::arrayBuffer);
                PHGL::glNormalPointer(a->type, a->stride, (const GLvoid*)a->offset);
                mask |= (1 << PHIMAGEATTRIBUTE_NRM);
            }
            
            gm->setGLAttributeStates(mask);
        }
        gm->bindVBO(vbo, PHGLVBO::arrayBuffer);
    }
    PHGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO?(elementVBO->vbo):0); 
}

void PHGLVertexArrayObject::fakeUnbind()
{
    if (elementVBO)
    {
        PHGLVertexBufferObject * vbo = gm->boundVBO(PHGLVBO::elementArrayBuffer);
        PHGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo?(vbo->vbo):0);
    }
}

void PHGLVertexArrayObject::vertexPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset, PHGLVertexBufferObject * vbo)
{
    if (vao)
    {
        bind_begin
         
        PHGLVBO * oldvbo = gm->boundVBO(PHGLVBO::arrayBuffer);
        vbo->bindTo(PHGLVBO::arrayBuffer);

        if (!(gm->useShaders()))
        {
            switch (index) {
                case PHIMAGEATTRIBUTE_POS:
                    PHGL::glEnableClientState(GL_VERTEX_ARRAY);
                    PHGL::glVertexPointer(size, type, stride, (const GLvoid *)offset);
                    break;
                case PHIMAGEATTRIBUTE_TXC:
                    PHGL::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    PHGL::glTexCoordPointer(size, type, stride, (const GLvoid *)offset);
                    break;
                case PHIMAGEATTRIBUTE_CLR:
                    PHGL::glEnableClientState(GL_COLOR_ARRAY);
                    PHGL::glColorPointer(size, type, stride, (const GLvoid *)offset);
                    break;
                case PHIMAGEATTRIBUTE_NRM:
                    PHGL::glEnableClientState(GL_NORMAL_ARRAY);
                    PHGL::glNormalPointer(type, stride, (const GLvoid *)offset);
                    break;
            }
        } else
        {
            PHGL::glEnableVertexAttribArray(index);
            PHGL::glVertexAttribPointer(index, size, type, normalized, stride, (const GLvoid *)offset);
        }
        
        gm->bindVBO(oldvbo, PHGLVBO::arrayBuffer);
        
        bind_end
        
    } else {
        attribute * a = new attribute;
        a->size = size;
        a->type = type;
        a->normalized = normalized;
        a->stride = stride;
        a->offset = offset;
        a->vbo = vbo;
        attribute * o = NULL;
        if ((o = attributes[index]))
            delete o;
        attributes[index] = a;
    }

    PHGLVBO * & v = attributeVBOs[index];
    if (vbo)
        vbo->retain();
    if (v)
        v->release();
    v = vbo;
}

void PHGLVertexArrayObject::disableAttribute(GLuint index)
{
    if (vao)
    {
        bind_begin
        
        if (!(gm->useShaders()))
        {
            switch (index) {
                case PHIMAGEATTRIBUTE_POS:
                    PHGL::glDisableClientState(GL_VERTEX_ARRAY);
                    break;
                case PHIMAGEATTRIBUTE_TXC:
                    PHGL::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    break;
                case PHIMAGEATTRIBUTE_CLR:
                    PHGL::glDisableClientState(GL_COLOR_ARRAY);
                    break;
                case PHIMAGEATTRIBUTE_NRM:
                    PHGL::glDisableClientState(GL_NORMAL_ARRAY);
                    break;
            }
        } else
            PHGL::glDisableVertexAttribArray(index);
        
        bind_end
    } else 
    {
        map<GLuint, attribute*>::iterator it = attributes.find(index);
        if (it!=attributes.end())
        {
            delete it->second;
            attributes.erase(it);
        }
    }
    map<GLuint, PHGLVertexBufferObject *>::iterator it = attributeVBOs.find(index);
    if (it!=attributeVBOs.end())
    {
        if (it->second)
            it->second->release();
        attributeVBOs.erase(it);
    }
}

void PHGLVertexArrayObject::setDrawElements(GLenum _mode, GLsizei _count, GLenum _type, size_t _offset)
{
    drawType = drawElements;
    mode = _mode;
    count = _count;
    type = _type;
    offset = _offset;
}

void PHGLVertexArrayObject::setDrawArrays(GLenum _mode, GLint _first, GLsizei _count)
{
    drawType = drawArrays;
    mode = _mode;
    first = _first;
    count = _count;
}

void PHGLVertexArrayObject::draw()
{
    bind_begin_draw
    switch (drawType)
    {
        case drawArrays:
            PHGL::glDrawArrays(mode, first, count);
            break;
        case drawElements:
            PHGL::glDrawElements(mode, count, type, (const GLvoid*)offset);
            break;
    }
    bind_end
}

PHGLVBO * PHGLVertexArrayObject::attributeVBO(GLuint index)
{
    map<GLuint, PHGLVBO*>::iterator it = attributeVBOs.find(index);
    if (it == attributeVBOs.end()) return NULL;
    return it->second;
}

void PHGLVertexArrayObject::setElementArrayVBO(PHGLVBO * vbo)
{
    if (bound)
        PHGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo?(vbo->vbo):0);
    if (vbo)
        vbo->retain();
    if (elementVBO)
        elementVBO->release();
    elementVBO = vbo;
}

void PHGLVertexArrayObject::bind()
{
    gm->bindVAO(this);
}

void PHGLVertexArrayObject::unbind()
{
    if (bound)
        gm->bindVAO(NULL);
}
