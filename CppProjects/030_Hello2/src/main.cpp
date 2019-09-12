//
//  main.cpp
//  SdlTest
//
//  Created by frankie on 01/03/2019.
//  Copyright © 2019 xtrigger. All rights reserved.
//

#include <iostream>

#if( __EMSCRIPTEN__  )
#include <emscripten.h>
#include <GLES3/gl3.h>
#else
#include <OpenGL/gl3.h>
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "SDL.h"
#pragma clang diagnostic pop

#include "stuff.h"

float Random( void ){
  return static_cast< float >( rand() ) / RAND_MAX;
}


template< int K, typename T >
struct tVec;



template< typename T >
struct tVec< 2, T > {
  union {
    struct {
      float x;
      float y;
    };
    float v[2];
  };
  T operator[]( int p ) const { return v[p]; }
  T & operator[]( int p ) { return v[p]; }
};



template< typename T >
struct tVec< 3, T > {
  union {
    struct {
      float x;
      float y;
      float z;
    };
    float v[3];
  };
  T operator[]( int p ) const { return v[p]; }
  T & operator[]( int p ) { return v[p]; }
};



template< typename T >
struct tVec< 4, T > {
  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };
    float v[4];
  };
  T operator[]( int p ) const { return v[p]; }
  T & operator[]( int p ) { return v[p]; }
};

using tVec2 = tVec<2,float>;
using tVec3 = tVec<3,float>;
using tVec4 = tVec<4,float>;

#define BIN_OP( OP )  \
template< int K, typename T > \
tVec< K, T > operator OP( tVec< K, T > p1, tVec< K, T > p2 ){ \
  auto  res = tVec< K, T >{}; \
  for( auto c = 0; c != K; ++c )  \
    res[c] = p1[c] OP p2[c]; \
  return res; \
}
BIN_OP( + )
BIN_OP( - )

template< int K, typename T > 
T Dot( tVec< K, T > p1, tVec< K, T > p2 ){
  auto res = T{};
  for( auto c = 0; c != K; ++c )
    res = p1[c] * p2[c];
  return res;
}

template< int K, typename T > 
tVec< K, T > operator*( tVec< K, T > p1, T p2 ){
  for( auto c = 0; c != K; ++c )
    p1[c] *= p2;
  return p1;
}

template< int K, typename T > 
tVec< K, T > operator*( T p1, tVec< K, T > p2 ){
  for( auto c = 0; c != K; ++c )
    p2[c] *= p1;
  return p2;
}



template< int R, int C, typename T >
struct tMtx {
  tVec< R, T >  m[C];
  tVec< R, T > operator[]( int p ) const { return m[p]; }
  tVec< R, T > & operator[]( int p ) { return m[p]; }
};


template< int R1, int C1R2, int C2, typename T >
tMtx< R1, C2, T > operator*( tMtx< R1, C1R2, T > const & p1, tMtx< C1R2, C2, T > const & p2 ){
  auto res = tMtx< R1, C2, T >{};
  for( auto c = 0; c != C2; ++c )
    for( auto i = 0; i != C1R2; ++i )
      res[c] = res[c] + p1[i] * p2[c][i];
  return res;
}



GLuint Shader( GLenum pShType, char const * pStr ){

  auto sh = glCreateShader(pShType);
  char const * arr[] = { "#version 100", pStr };
  glShaderSource(sh, 2, arr, nullptr );
  glCompileShader( sh );
  
  auto status = GLint{};
  
  glGetShaderiv( sh, GL_COMPILE_STATUS, &status);
  
  if( status != GL_TRUE ){
    auto len = GLint{};
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
    auto buff = std::string{};
    buff.resize(len+1);
    glGetShaderInfoLog(sh, len, &len, &buff[0]);
    std::cout << "Shader error\n" << buff << '\n';
    glDeleteShader(sh);
    return 0;
  }
  return sh;
}

GLuint CreateLinkProgram( GLuint pVsh, GLuint pFsh ){

  auto pg = glCreateProgram();
  
  glAttachShader(pg, pVsh);
  glAttachShader(pg, pFsh);
  glLinkProgram(pg);
  
  auto status = GLint{};
  
  glGetProgramiv( pg, GL_LINK_STATUS, &status);
  
  if( status != GL_TRUE ){
    auto len = GLint{};
    glGetProgramiv( pg, GL_INFO_LOG_LENGTH, &len);
    auto buff = std::string{};
    buff.resize(len+1);
    glGetProgramInfoLog(pg, len, &len, &buff[0]);
    std::cout << "Program error\n" << buff << '\n';
    glDeleteProgram(pg);
    return 0;
  }
  return pg;
}


auto vsh = R"(
  attribute vec4 aPos;
  varying vec4 vPos;
  void main(){
    vPos = aPos;
    gl_Position = aPos;
  }
)";


auto fsh = R"(
precision highp float;
uniform float time;

varying vec4 vPos;

float Rnd( vec2 p )
{
    return fract(sin(dot(p,vec2(793.791,9916.48)))*73469.93);
}

float NoiseB( vec2 p )
{
    vec2    i = floor( p );
    vec2    f = smoothstep(0.0,1.0,fract( p ));
    float   r00 = Rnd( i + vec2( 0.0, 0.0 ) );
    float   r10 = Rnd( i + vec2( 1.0, 0.0 ) );
    float   r01 = Rnd( i + vec2( 0.0, 1.0 ) );
    float   r11 = Rnd( i + vec2( 1.0, 1.0 ) );    
    float   v0 = mix( r00, r10, f.x );
    float   v1 = mix( r01, r11, f.x );
    
    return mix( v0,v1,f.y);
}

float Noise( vec2 p )
{
    return abs( NoiseB(p) );
}

float FractNoise( vec2 p )
{
    float r = Noise( p ) * 0.5 +
           Noise( p * 2.0 ) * 0.25 +
           Noise( p * 4.0 ) * 0.125 + 
           Noise( p * 8.0 ) * 0.0625;
    
    return r * 1.066666666666667;
}

float Pattern( vec2 uv )
{
    return sin( uv.y * 12.0 + uv.x *10.0) + sin( uv.y * 39775.0 + uv.x *52.0) + sin( uv.y * 55.0 + uv.x *1.0);
}

void main(void)
{
    vec2    resolution = vec2( 1024.0, 768.0 );
    vec2    uv = ( gl_FragCoord.xy / resolution.xy * 2.0 - 1.0 ) * resolution.xy / resolution.y;
    float   r = length( uv );
    float   a1 = atan( uv.y, uv.x );
    float   a2 = atan( uv.y, -uv.x );
    float   d = time * 0.5+ 1.0/r;
    
    
    
    float v1 = Pattern( vec2( d * 0.8 + Noise(vec2(a1,time*0.25)) * 1.0 ,(a1)*0.004+time*0.01  ) );
    float v2 = Pattern( vec2( d * 0.8 + Noise(vec2(a2 + 34.4,time*0.25)) * 1.0 ,(a2)*0.004+time*0.01  ) );

    float v = mix( v1,v2,abs(a1)*0.318309886183791);

    gl_FragColor = vec4( 0.9,0.1,1.0,1.0)*vec4( v ) * smoothstep( 0.15,0.8,r);
}

)";

static auto wind = static_cast<SDL_Window*>(nullptr);
static auto quit = false;
static int timeLoc = 0;
static float myTime = 0.0f;


void Iter( void ){
    auto event = SDL_Event{};
    
    while( SDL_PollEvent(&event) != 0 ){
      if( event.type == SDL_QUIT ){
        quit = true;
      }
    }
    myTime += 0.016f;
    glUniform1f( timeLoc, myTime);
    glClearColor(Random(), Random(), Random(), Random());
    glClear( GL_COLOR_BUFFER_BIT );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );
  assert( glGetError() == GL_NO_ERROR );
    SDL_GL_SwapWindow(wind);
}

int main(int argc, const char * argv[]) {
    DoSomething();
  std::cout << "NEW VERSION\n";
   auto err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER );
//  auto err = SDL_Init(SDL_INIT_VIDEO  | SDL_INIT_TIMER );
  assert( err == 0 );

#if( __EMSCRIPTEN__ )

#else
  err = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
  assert( err == 0 );
  err = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2 );
  assert( err == 0 );
  err = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
  assert( err == 0 );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  assert( err == 0 );
#endif

  wind = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL);
  assert( wind );
  
  auto ctxt = SDL_GL_CreateContext( wind );
  std::cout << SDL_GetError() << std::endl;
  assert( ctxt );
    
  err= SDL_GL_SetSwapInterval(1);
  assert( err == 0 );

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // OPENGL STUFF
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert( glGetError() == GL_NO_ERROR );
  auto vshId = Shader( GL_VERTEX_SHADER, vsh );
  auto fshId = Shader( GL_FRAGMENT_SHADER, fsh );
  auto progId = CreateLinkProgram( vshId, fshId );
  tVec4 vtxs[] = { 
    {{{  1.0f,  1.0f, 0.0f, 1.0f }}},
    {{{ -1.0f,  1.0f, 0.0f, 1.0f }}},
    {{{ -1.0f, -1.0f, 0.0f, 1.0f }}},
    {{{  1.0f, -1.0f, 0.0f, 1.0f }}}
  };
  assert( glGetError() == GL_NO_ERROR );
  uint32_t idxs[] = { 0,1,2,0,2,3 };
  auto vao = GLuint{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  auto vbo = GLuint{};
  glGenBuffers(1, &vbo);
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( vtxs ), vtxs, GL_STATIC_DRAW );
  auto ibo = GLuint{};
  glGenBuffers( 1, &ibo);
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( idxs ), idxs,  GL_STATIC_DRAW );
  assert( glGetError() == GL_NO_ERROR );
  auto aPosLoc = glGetAttribLocation(progId, "aPos");
  glEnableVertexAttribArray(aPosLoc);
  glVertexAttribPointer( aPosLoc, 4, GL_FLOAT, GL_FALSE, sizeof( tVec4 ), nullptr );
  assert( glGetError() == GL_NO_ERROR );
  glUseProgram(progId);
  assert( glGetError() == GL_NO_ERROR );
  glViewport( 0, 0, 1024, 768);
  assert( glGetError() == GL_NO_ERROR );
  
  timeLoc = glGetUniformLocation( progId, "time");
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // LOOP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert( progId != 0 );

#if( __EMSCRIPTEN__ )
  emscripten_set_main_loop( &Iter,0,1);
#else
  while( !quit ){
    Iter();
  }
#endif
  return 0;
}


