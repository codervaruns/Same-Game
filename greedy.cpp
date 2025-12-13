#include <bits/stdc++.h>
using namespace std;

int main() {
    vector<vector<char>>grid={{'G','G','W','R'},{'G','W','W','R'},{'W','W','W','R'}};
    int m=3;
    int n=4;
    vector<vector<bool>>totVis(m,vector<bool>(n,true));
    bool cnt=true;
    int cscore=0;
   priority_queue<
    tuple<int,char,int,int>,
    vector<tuple<int,char,int,int>>,
    less<tuple<int,char,int,int>>
> colorVMain;
   // local
    vector<vector<bool>>vis=totVis;//copy of vis
    priority_queue<
    tuple<int,char,int,int>,
    vector<tuple<int,char,int,int>>,
    less<tuple<int,char,int,int>>
> colorV=colorVMain;//copy of colorVMain
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            int x=i;
            int y=j;
            queue<pair<int,int>>q;
            q.push({x,y});
            int mxSize=1;
            if(vis[x][y]==true){
            vis[x][y]=false;
            char cr=grid[x][y];
            while(!q.empty()){
                int x1=q.front().first;
                int y1=q.front().second;
                q.pop();
                vector<vector<int>>dir={{-1,0},{0,1},{1,0},{0,-1}};
                int n1=4;
                for(int i=0;i<n1;i++){
                    int x2=x1+dir[i][0];
                    int y2=y1+dir[i][1];
                    if(x2>=0 && x2<m && y2>=0 && y2<n){
                        if(vis[x2][y2]==true && grid[x2][y2]==cr){
                            vis[x2][y2]=false;
                            q.push({x2,y2});
                            mxSize++;
                        }
                    }
                }
              }//while
              if(mxSize!=1){
              colorV.push({mxSize,cr,x,y});
              }
              else{
                  totVis[x][y]=false;
              }
            }

        }
    }
     int sz=colorV.size();
     auto[bSize,bColor,bx,by]=colorV.top();
     colorVMain.pop();
     while(!colorV.empty()){
         auto[a,b,c,d]=colorV.top();
         cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
         colorV.pop();
     }
     cout<<endl;
     cscore+=(bSize-2)*(bSize-2);
     queue<pair<int,int>>q;
     q.push({bx,by});
     totVis[bx][by]=false;
     vector<vector<int>>curStore(m,vector<int>());
     while(!q.empty()){
                int x1=q.front().first;
                int y1=q.front().second;
                q.pop();
                vector<vector<int>>dir={{-1,0},{0,1},{1,0},{0,-1}};
                int n1=4;
                for(int i=0;i<n1;i++){
                    int x2=x1+dir[i][0];
                    int y2=y1+dir[i][1];
                    if(x2>=0 && x2<m && y2>=0 && y2<n){
                        if(totVis[x2][y2]==true && grid[x2][y2]==bColor){
                            totVis[x2][y2]=false;
                            curStore[x2].push_back(y2);
                            q.push({x2,y2});
                        }
                    }
                }
      }//while

      // grid state before gravity
      cout << "Grid before gravity:\n";
      for(int i=0;i<m;i++){
          for(int j=0;j<n;j++){
              if(totVis[i][j]) cout << grid[i][j] << " ";
              else cout << ". ";
          }
          cout << endl;
      }
      cout << endl;

      //grid gravity logic
      for(int j=0;j<n;j++){
        int pos = m-1;
        for(int i=m-1;i>=0;i--){
            if(totVis[i][j]==true){
                if(i!=pos){
                    grid[pos][j]=grid[i][j];
                    totVis[pos][j]=true;
                    totVis[i][j]=false;
                }
                pos--;
            }
        }
    }

    int col=0;
    for(int j=0;j<n;j++){
        bool hasTiles=false;
        for(int i=0;i<m;i++){
            if(totVis[i][j]==true){
                hasTiles=true;
                break;
            }
        }
        if(hasTiles){
            if(j!=col){
                for(int i=0;i<m;i++){
                    grid[i][col]=grid[i][j];
                    totVis[i][col]=totVis[i][j];
                    totVis[i][j]=false;
                }
            }
            col++;
        }
    }

    // grid state after gravity
      cout << "Grid after gravity:\n";
      for(int i=0;i<m;i++){
          for(int j=0;j<n;j++){
              if(totVis[i][j]) cout << grid[i][j] << " ";
              else cout << ". ";
          }
          cout << endl;
      }
      cout << endl;
      
      if(colorVMain.empty()){
          cnt=false;
         // break;
      }
     return 0;
}