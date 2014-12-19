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
#define vectorave 0.7

struct vec{
  float nx;
  float ny;
  float nz;
};

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
    }else if(p.mx == mx && abs(p.my - my) == 1 && p.mz == mz){
      return p.my - my == 1 ? 5 : 6;
    }else if(p.mx == mx && p.my == my && abs(p.mz - mz) == 1){
      return  (p.mz - mz) == 1 ? 1 : 2;
    }else if(abs(p.mx - mx) == 1 && abs(p.my - my) == 1 && p.mz == mz){
      if(p.mx - mx == 1){
        return  (p.my - my) == -1 ? 7 : 8;
      }else{
        return  (p.my - my) == -1 ? 9 : 10;
      }
    }else if(abs(p.mx - mx) == 1 && p.my == my && abs(p.mz - mz) == 1){
      if(p.mx - mx == 1){
        return  (p.mz - mz) == -1 ? 11 : 12;
      }else{
        return  (p.mz - mz) == -1 ? 13 : 14;
      }
    }else if(p.mx == mx && abs(p.my - my) == 1 && abs(p.mz - mz) == 1){
      if(p.my - my == 1){
        return  (p.mz - mz) == -1 ? 15 : 16;
       }else{
        return  (p.mz - mz) == -1 ? 17 : 18;
      }
    }else if(abs(p.mx - mx) == 1 && abs(p.my - my) == 1 && abs(p.mz - mz) == 1){
      if(p.mx - mx == 1){
        if(p.my - my == -1){
          return  (p.mz - mz) == -1 ? 19 : 20;
        }else{
          return  (p.mz - mz) == -1 ? 21 : 22;
        }
      }else{
        if(p.my - my == -1){
          return  (p.mz - mz) == -1 ? 23 : 24;
        }else{
          return  (p.mz - mz) == -1 ? 25 : 26;
        }
      }
    }else{
      return 27;
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

bool SimVec(point p,vec n){
  float inn = p.nx*n.nx + p.ny*n.ny + p.nz*n.nz;
  return inn < 0 ? false : true;
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
  queue<point> ops3;
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



  //濃度で削除
  for(int i=1;i<(int)points.size();i++){
    if(points[i] == points[i-1]){
      count++;
    }
    else{
      if(count >= num){
        for(int j=0;j<count;j++){
          ops1.push(points[i-j-1]);
        }
        vnum++;
      }
      count = 0;
    }
  }

  cout << "voxel all:" << vnum << endl;


  //近傍のボクセルの量で削除
  while(!ops1.empty()){
    pv.p = ops1.front();
    pv.v = 3;
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
        }else if(p.near(ops1.front()) < 27){
          if(!(pp == ops1.front())){
            pp = ops1.front();
            pv.p = pp;
            pv.v = pp.near(ops1.front());
            dfs.push(pv);
          }
          ops2.push(ops1.front());
        }else{
          ops2.push(ops1.front());
        }
        ops1.pop();
      }
      count++;
      ops1 = ops2;
      while(!ops2.empty()){
        ops2.pop();
      }
    }
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    while(!ps.empty()){
      if(count >= (int)(vnum * density)){
        ps.front().r = r;
        ps.front().g = g;
        ps.front().b = b;
        ops3.push(ps.front());
        //opoints.push_back(ps.front());
      }
      ps.pop();
    }

    cout << "voxel count : " << count << endl;
  }



  //ベクトルの平均で削除
  while(!ops3.empty()){
    float sumnx=0;
    float sumny=0;
    float sumnz=0;
    vec n;
    p = ops3.front();
    while(ops3.front() == p && !ops3.empty()){
      ops1.push(ops3.front());
      sumnx += ops3.front().nx;
      sumny += ops3.front().ny;
      sumnz += ops3.front().nz;
      ops3.pop();
    }
    n.nx = sumnx / ops1.size();
    n.ny = sumny / ops1.size();
    n.nz = sumnz / ops1.size();
    while(!ops1.empty()){
      if(SimVec(ops1.front(),n)){
        opoints.push_back(ops1.front());
      }
      ops1.pop();
    }
  }



  //出力
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
