#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>
#include <iterator>
#include <queue>
#include <float.h>

using namespace std;

#define voxel 1
#define density 0.3
#define densityvoxel 0.001

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
  int mx;
  int my;
  int mz;

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
    if(abs(p.mx - mx) == 1 && p.my == my && p.mz == mz){
      return p.mx - mx == 1 ? 3 : 4;
    }else if(p.mx == mx && abs(p.my - my*voxel) == 1 && p.mz == mz){
      return p.my - my == 1 ? 5 : 6;
    }else if(p.mx == mx && p.my == my && abs(p.mz -mz) == 1){
      return  (p.mz - mz) == 1 ? 1 : 2;
    }else{
      return 7;
    }
  }
};

struct pointvector{
  point p;
  int v;
};

int label(float x){
  return (int)( x < 0.0 ? x-0.5 : x+0.5 );
}

int main(int argc,char *argv[]){
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
  queue<point> ops1;
  queue<point> ops2;
  queue<point> ps;
  stack<pointvector> dfs;
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
      num = atoi(s.c_str()) * densityvoxel;
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
    p.mx = label(p.x*voxel);
    p.my = label(p.y*voxel);
    p.mz = label(p.z*voxel);
    points.push_back(p);
  }
  sort(points.begin(),points.end());

  for(int i=1;i<(int)points.size();i++){
    if(!(points[i-1] == points[i])){
      vnum++;
    }
  }

  for(int i=1;i<(int)points.size();i++){
    if(points[i] == points[i-1]){
      count++;
    }
    else{
      if(count >= num){
        for(int j=0;j<count;j++){
          ops1.push(points[i-j-1]);
        }
      }
      count = 0;
    }
  }

  while(!ops1.empty()){
    pv.p = ops1.front();
    pv.v = 3;
    cout << ops1.size() << endl;
    cout << ops2.size() << endl;
    dfs.push(pv);
    count = 0;
    while(!dfs.empty()){
      pv = dfs.top();
      dfs.pop();
      p = pv.p;
      pp = p;
      while(!ops1.empty()){
        if(ops1.front() == p){
          ps.push(ops1.front());
        }else if(p.near(ops1.front()) < 7){
          if(!(pp == ops1.front())){
            count++;
            pp = ops1.front();
            pv.p = pp;
            pv.v = pp.near(ops1.front());
            dfs.push(pv);
            cout << dfs.size() << endl;
          }else{
            ops2.push(ops1.front());
          }
        }else{
          ops2.push(ops1.front());
        }
        ops1.pop();
      }
      cout << "voxel count : " << count << endl;
      cout << ops2.size() << endl;

      while(!ps.empty()){
        if(count >= (int)(vnum * density)){
          opoints.push_back(ps.front());
        }
        ps.pop();
      }
    }
    ops1 = ops2;
    while(!ops2.empty()){
      ops2.pop();
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
