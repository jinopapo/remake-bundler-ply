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
#include <set>

using namespace std;

#define voxel 1
#define density 0.5
#define densityvoxel 1/voxelnum
#define voxelnum 2000

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
        return mz < right.mz;
      }
      else{
        return my < right.my;
      }
    }
    else{
      return mx < right.mx;
    }
  }

  bool operator>(const point& right) const
  {
    if(mx == right.mx){
      if(my == right.my){
        return mz > right.mz;
      }
      else{
        return my > right.my;
      }
    }
    else{
      return mx > right.mx;
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


int MakeCell(queue<point> iq){
  int vcount=0;
  point p;
  vcount = 0;
  while(!iq.empty()){
    if(!(p == iq.front())){
      p = iq.front();
      vcount++;
    }
    iq.pop();
  }
  return vcount;
}

//濃度で削除
queue<point> Density(queue<point> iq,int& vnum,int num){
  queue<point> oq;
  queue<point> q,q1;
  point p;
  vnum = 0;
  while(!oq.empty()){
    oq.pop();
  }
  q = iq;
  p = q.front();
  q1.push(q.front());
  q.pop();
  while(!q.empty()){
    if(p == q.front()){
      q1.push(q.front());
    }else{
      p = q.front();
      if((int)q1.size() >= num){
/*          int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;*/
        while(!q1.empty()){
          /*points[i-j-1].r = r;
            points[i-j-1].g = g;
            points[i-j-1].b = b;*/
          oq.push(q1.front());
          q1.pop();
        }
        vnum++;
      }else{
        while(!q1.empty()){
          q1.pop();
        }
      }
      q1.push(p);
    }
    q.pop();
  }
  cout << "voxel all:" << vnum << endl;
  return oq;
}


//近傍のボクセルの量で削除
queue<point> Consolidated(queue<point> iq,int vnum){
  queue<point> oq;
  queue<point> q,q1;
  set<point> map;
  pointvector pv;
  stack<pointvector> dfs;
  int count = 0;
  point p,pp;
  while(!iq.empty()){
    pv.p = iq.front();
    pv.v = 3;
    if(map.insert(pv.p).second){
      dfs.push(pv);
    }
    count = 0;
    while(!dfs.empty()){
      pv = dfs.top();
      dfs.pop();
      p = pv.p;
      pp = p;
      while(!iq.empty()){
        if(iq.front() == p){
          q1.push(iq.front());
        }else if(p.near(iq.front()) < 27){
          if(!(pp == iq.front())){
            pp = iq.front();
            pv.p = pp;
            pv.v = pp.near(iq.front());
            if(map.insert(pv.p).second){
              dfs.push(pv);
            }
          }
          q.push(iq.front());
        }else{
          q.push(iq.front());
        }
        iq.pop();
      }
      count++;
      iq = q;
      while(!q.empty()){
        q.pop();
      }
    }
    /*int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;*/
    while(!q1.empty()){
      if(count >= (int)(vnum * density)){
        /*ps.front().r = r;
        ps.front().g = g;
        ps.front().b = b;*/
        /*ps.front().r = ps.front().mx;
        ps.front().g = ps.front().my;
        ps.front().b = ps.front().mz;*/
        oq.push(q1.front());
      }
      q1.pop();
    }

    cout << "voxel count : " << count << endl;
  }
  return oq;
}

//ベクトルの平均で削除
queue<point> Normal(queue<point> iq){
  queue<point> oq;
  queue<point> q;
  while(!iq.empty()){
    float sumnx=0;
    float sumny=0;
    float sumnz=0;
    vec n;
    point p;
    p = iq.front();
    while(iq.front() == p && !iq.empty()){
      q.push(iq.front());
      sumnx += iq.front().nx;
      sumny += iq.front().ny;
      sumnz += iq.front().nz;
      iq.pop();
    }
    n.nx = sumnx / q.size();
    n.ny = sumny / q.size();
    n.nz = sumnz / q.size();
    while(!q.empty()){
      if(SimVec(q.front(),n)){
        oq.push(q.front());
      }
      q.pop();
    }
  }
  return oq;
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
  queue<point> opoints;
  queue<point> ops1;
  queue<point> ops2;
  queue<point> ops3;
  queue<point> ps;
  point p;
  int num;
  bool flag = false;
  int vnum = 1;
  int vcount=0;
  int n = voxel;

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
      num = atoi(s.c_str());
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

  while(vcount < voxelnum){
    queue<point> ops;
    for(int i=0;i<(int)points.size();i++){
      points[i].mx = label(points[i].x*n);
      points[i].my = label(points[i].y*n);
      points[i].mz = label(points[i].z*n);
    }

    sort(points.begin(),points.end());

    for(auto i:points){
      ops.push(i);
    }
    vcount = MakeCell(ops);
    n++;
    ops1 = ops;
  }

  num /= vcount;
  ops1 = Density(ops1,vnum,num);

  ops2 = Normal(ops1);

  ops2 = Density(ops2,vnum,num);

  opoints = Consolidated(ops2,vnum);

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

  while(!opoints.empty()){
    ofs << opoints.front().x << " ";
    ofs << opoints.front().y << " ";
    ofs << opoints.front().z << " ";
    ofs << opoints.front().nx << " ";
    ofs << opoints.front().ny << " ";
    ofs << opoints.front().nz << " ";
    ofs << opoints.front().r << " ";
    ofs << opoints.front().g << " ";
    ofs << opoints.front().b << endl;
    opoints.pop();
  }

  return 0;
}
