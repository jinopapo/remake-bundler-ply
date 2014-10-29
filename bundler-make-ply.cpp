#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>
#include <iterator>

using namespace std;

#define voxel 100
#define density 0.3

struct point
{
  float x;
  float y;
  float z;
  float nx;
  float ny;
  float nz;
  int r;
  int g;
  int b;
  float mx;
  float my;
  float mz;

  bool operator<(const point& right) const
  {
    if(mx == right.mx){
      if(my == right.my){
        return mz <right.mz;
      }
      else{
        return my < right.my;
      }
    }
    else{
      return mx < right.mx;
    }
  }

  bool operator==(const point& right) const
  {
    return mx == right.mx && my == right.my && mz == right.mz;
  }

  int near(point p)
  {
    if(abs((int)(p.mx*voxel) -(int)(mx*voxel)) == 1
    && (int)(p.my*voxel) -(int)(my*voxel) == 0
    && (int)(p.mz*voxel) -(int)(mz*voxel) == 0){
      if((int)(p.mx*voxel) -(int)(mx*voxel) == 1){
        return 3;
      }
      else{
        return 4;
      }
    }
    else if((int)(p.mx*voxel) -(int)(mx*voxel) == 0
    && abs((int)(p.my*voxel) -(int)(my*voxel)) == 1
    && (int)(p.mz*voxel) -(int)(mz*voxel) == 0){
      if((int)(p.my*voxel) -(int)(my*voxel) == 1){
        return 5;
      }
      else{
        return 6;
      }
    }
    else if((int)(p.mx*voxel) -(int)(mx*voxel) == 0
    && (int)(p.my*voxel) -(int)(my*voxel) == 0
    && abs((int)(p.mz*voxel) -(int)(mz*voxel)) == 1){
      if((int)(p.mz*voxel) -(int)(mz*voxel) == 1){
        return 1;
      }
      else{
        return 2;
      }
    }
  }
};

struct pointvector
{
  point p;
  int v;
};

int label(float x)
{
  return (int)( x < 0.0 ? x-0.5 : x+0.5 );
}

int main(int argc,char *argv[])
{

  if(argc < 2 ){
    cout << "bundler-make-ply --inputFileName" << endl;
    cout << "bundler-make-ply --inputFileName --addOutPutFileName" << endl;
    return -1;
  }

  string s;
  ifstream ifs;
  ofstream ofs;
  ifstream iofs;
  vector<point> points;
  vector<point> opoints;
  vector<point> ps;
  vector<point> ops;
  vector<point>::iterator head;
  vector<point>::iterator end;
  stack<pointvector> dfs;
  ghiehgie
  point p,pp;
  pointvector pv;
  int count=0;
  int num;
  bool flag = false;
  int vnum = 1;

  ifs.open(argv[1],ios::in);
  if(!ifs.is_open()){
    cout << "No such a file" << endl;
    return -1;
  }

  if(argc == 3){
    ofs.open(argv[2],ios::out);
    iofs.open(argv[2],ios::in);
  }
  else{
    ofs.open("out.ply",ios::out);
  }

  do{
    ifs >> s;
    if(flag){
      num = atoi(s.c_str()) * 0.001;
      flag=false;
    }
    if(s == "vertex"){
      flag = true;
    }
  }while(s!="end_header");

  while(!ifs.eof()){
    ifs >> p.x;
    ifs >> p.y;
    ifs >> p.z;
    ifs >> p.nx;
    ifs >> p.ny;
    ifs >> p.nz;
    ifs >> p.r;
    ifs >> p.g;
    ifs >> p.b;
    p.mx = label(p.x*voxel)/voxel;
    p.my = label(p.y*voxel)/voxel;
    p.mz = label(p.z*voxel)/voxel;
    points.push_back(p);
  }
  sort(points.begin(),points.end());

  for(int i=1;i<(int)points.size();i++){
    if(!(points[i-1] == points[i])){
      vnum++;
    }
  }

  cout << vnum << endl;

  for(int i=1;i<(int)points.size();i++){
    if(points[i] == points[i-1]){
      count++;
    }
    else{
      if(count >= num){
        for(int j=0;j<count;j++){
          ops.push_back(points[i-j-1]);
        }
      }
      count = 0;
    }
  }

  //vectorからqueueに変更
  //queueを２つ用意して交互に使う
  //前1後ろ2右3左4上5下6
  while(!ops.empty()){
    pv.p = ops[0];
    pv.v = 3;
    dfs.push(pv);
    head = ops.begin();
    end = ops.begin();
    count = 0;
    while(!dfs.empty()){
      pv = dfs.top();
      dfs.pop();
      p = pv.p;
      pp = p;
      while(head != ops.end()){
        if(*end == p){
          end++;
        }else if(p.near(*end)  > 7){
          if(!(pp == *end)){
            count++;
            pp = *end;
            pv.p = pp;
            pv.v = pp.near(*end);
            dfs.push(pv);
          }
          end++;
        }
        else{
          if(head != end){
            copy(head,end,back_inserter(ps));
            ops.erase(head,end);
            head = end;
          }
          else{
            head++;
            end++;
          }
        }
      }
      if(count >= (int)(vnum * density)){
        opoints.insert(opoints.end(),ps.begin(),ps.end());
      }
      else{
        ps.clear();
      }
    }
  }

  ofs << "ply" <<endl;
  ofs << "format ascii 1.0" << endl;
  ofs << "element vertex " << opoints.size() << endl;
  ofs << "property float x" << endl;
  ofs << "property float y" << endl;
  ofs << "property float z" << endl;
  ofs << "property float nx" << endl;
  ofs << "property float ny" << endl;
  ofs << "property float nz" << endl;
  ofs << "property uchar diffuse_red" << endl;
  ofs << "property uchar diffuse_green" << endl;
  ofs << "property uchar diffuse_blue" << endl;
  ofs << "end_header" << endl;

  for(int i=0;i<(int)opoints.size();i++){
    ofs << opoints[i].x << " ";
    ofs << opoints[i].y << " ";
    ofs << opoints[i].z << " ";
    ofs << opoints[i].nx << " ";
    ofs << opoints[i].ny << " ";
    ofs << opoints[i].nz << " ";
    ofs << opoints[i].r << " ";
    ofs << opoints[i].g << " ";
    ofs << opoints[i].b << endl;
  }

  return 0;
}
