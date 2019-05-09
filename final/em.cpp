#include <stdio.h>
#include <math.h>
#include <vector>

const int N = 2000000;
const int M = 27;
const int K = 8;
const double Zero = log(0);

template <class T>
struct SparseE
{
   int k;
   T v;
};
typedef SparseE<double> SparseP;
typedef SparseE<int> SparseI;

double bigram[M][M];
std::vector<SparseP> forward[N][K], backward[N][K], encode[M][K];
std::vector<SparseP> maxp[N][K];
std::vector<SparseI> trace[N][K];
double ecnt[M][K][M], esum[M][K], bcnt[M][M], pi[M];

int n;
int inp[N], out[N];

inline int c2n(char ch)
{
   if (ch == ' ') return 0;
   return ch - 96;
}

inline char n2c(int n)
{
   if (n == 0) return ' ';
   return n + 96;
}

inline double sumlog(double la, double lb)
{
   if (la < 0 && isinf(la)) return lb;
   if (lb - la > 15) return lb;
   return la + log(1 + exp(lb - la));
}


void EM_iter(int n)
{
   int t, i, j, k, l;

   for (j = 0; j < forward[0][1].size(); j++)
   {
      auto &x = forward[0][1][j];
      x.v = pi[x.k] + encode[inp[0]][1][j].v;
   }

   for (t = 1; t < n; t++)
   {
      int o = inp[t];
      // forward[n][0][0] = sum_i sum_c forward[n-1][i][c] 
      if (forward[t][0].size() > 0)
      {
         auto &x1 = forward[t][0][0];
         x1.v = Zero;
         for (i = 1; i < K; i++)
            for (auto &x0 : forward[t-1][i])
               x1.v = sumlog(x1.v, x0.v + bigram[x0.k][x1.k] + encode[o][0][0].v);
      }

      // forward[n][i][c] = sum_c' forward[n-1][i-1][c']
      for (i = 1; i < K; i++)
      {
         int sz1 = forward[t][i].size();
         for (j = 0; j < sz1; j++)
         {
            auto &x1 = forward[t][i][j];
            int i0 = i - 1;
            if (i0 == 0) i0 = K - 1;

            x1.v = Zero;
            for (auto &x0 : forward[t-1][i0])
               x1.v = sumlog(x1.v, x0.v + bigram[x0.k][x1.k] + encode[o][i][j].v);
            if (i == 1 && forward[t-1][0].size() > 0)
               x1.v = sumlog(x1.v, forward[t-1][0][0].v + bigram[0][x1.k] + encode[o][i][j].v);
         }
      }
   }

   for (t = n - 2; t >= 0; t--)
   {
      int o = inp[t+1];

      //backward[n][0][0] = sum_i sum_c backward[n+1][i][c]
      /*if (backward[t][0].size() > 0)
      {
         auto &x0 = backward[t][0][0];
         x0.v = Zero;
         //for (i = 1; i < K; i++)
         i = 1;
         {
            int sz1 = backward[t+1][i].size();
            for (j = 0; j < sz1; j++)
            {
               auto &x1 = backward[t+1][i][j];
               x0.v = sumlog(x0.v, x1.v + bigram[x0.k][x1.k] + encode[o][i][j].v);
            }
         }
      }*/

      // backward[n][i][c] = sum_c' backward[n+1][i+1][c']
      for (i = 0; i < K; i++)
      {
         for (auto &x0 : backward[t][i])
         {
            x0.v = Zero;
            int i1 = i + 1;
            if (i1 == K) i1 = 1;
            int sz1 = backward[t+1][i1].size();
            for (j = 0; j < sz1; j++)
            {
               auto &x1 = backward[t+1][i1][j];
               x0.v = sumlog(x0.v, x1.v + bigram[x0.k][x1.k] + encode[o][i1][j].v);
            }
            if (i > 0 && backward[t+1][0].size() > 0)
               x0.v = sumlog(x0.v, backward[t+1][0][0].v + bigram[x0.k][0] + encode[o][0][0].v);
         }
      }
   }

   for (i = 0; i < M; i++)
      for (j = 0; j < K; j++)
      {
         esum[i][j] = Zero;
         for (k = 0; k < M; k++)
            ecnt[i][j][k] = Zero;
      }

   for (t = 0; t < n; t++)
   {
      int o = inp[t];
      for (i = 0; i < K; i++)
      {
         int sz = forward[t][i].size();
         for (j = 0; j < sz; j++)
         {
            int k = forward[t][i][j].k;
            double tmp = forward[t][i][j].v + backward[t][i][j].v;
            ecnt[k][i][o] = sumlog(ecnt[k][i][o], tmp);
            esum[k][i] = sumlog(esum[k][i], tmp);
         }
      }
   } 

   double total = Zero;
   for (j = 0; j < forward[0][1].size(); j++)
   {
      int k = forward[0][1][j].k;
      double tmp = forward[0][1][j].v + backward[0][1][j].v;
      pi[k] = tmp;
      total = sumlog(total, tmp);
   }
   for (j = 0; j < forward[0][1].size(); j++)
   {
      int k = forward[0][1][j].k;
      if (!isinf(pi[k]))
         pi[k] -= total;
   }
   fprintf(stderr, "%lf\n", total);

   for (i = 0; i < M; i++)
      for (j = 0; j < M; j++)
         bcnt[i][j] = Zero;

   for (t = 0; t < n - 1; t++)
   {
      int o = inp[t+1];
      /*for (auto &x0 : forward[t][0])
         for (i = 1; i < K; i++)
         {
            int sz = backward[t+1][i].size();
            for (j = 0; j < sz; j++)
            {
               auto &x1 = backward[t+1][i][j];
               bcnt[x0.k][x1.k] = sumlog(bcnt[x0.k][x1.k], x0.v + bigram[x0.k][x1.k] + x1.v + encode[o][i][j].v);
            }
         }*/

      for (i = 0; i < K; i++) 
      {
         int i1 = i + 1;
         if (i1 == K) i1 = 1;
         for (auto &x0 : forward[t][i])
         {
            int sz = backward[t+1][i1].size();
            for (j = 0; j < sz; j++)
            {
               auto &x1 = backward[t+1][i1][j];
               bcnt[x0.k][x1.k] = sumlog(bcnt[x0.k][x1.k], x0.v + bigram[x0.k][x1.k] + x1.v + encode[o][i1][j].v);
            }
            if (i > 0 && backward[t+1][0].size() > 0)
               bcnt[x0.k][0] = sumlog(bcnt[x0.k][0], x0.v + bigram[x0.k][0] + backward[t+1][0][0].v + encode[o][0][0].v);
         }
      }
   }

   for (i = 0; i < M; i++)
      for (j = 0; j < K; j++)
         for (auto &x : encode[i][j])
         {
            x.v = ecnt[x.k][j][i];
            if (!isinf(x.v))
               x.v -= esum[x.k][j];
         }

   for (i = 0; i < M; i++)
   {
      double s = Zero;
      for (k = 0; k < K; k++)
         s = sumlog(s, esum[i][k]);
      for (j = 0; j < M; j++)
      {
         bigram[i][j] = bcnt[i][j];
         if (!isinf(bigram[i][j]))
            bigram[i][j] -= s;
      }
   }
}

void viterbi()
{
   int t, i, j, k;

   for (j = 0; j < maxp[0][1].size(); j++)
      maxp[0][1][j].v = pi[inp[0]] + encode[inp[0]][1][j].v;

   for (t = 1; t < n; t++)
   {
      int o = inp[t];
      if (maxp[t][0].size() > 0)
      {
         double mx = Zero;
         trace[t][0][0].v = -1;
         for (i = 1; i < K; i++)
         {
            int sz = maxp[t-1][i].size();
            for (j = 0; j < sz; j++)
            {
               double tmp = maxp[t-1][i][j].v + bigram[maxp[t-1][i][j].k][0];
               if (tmp > mx)
               {
                  mx = tmp;
                  trace[t][0][0].v = i * M + j;
               }
            }
         }
         maxp[t][0][0].v = mx + encode[o][0][0].v;
      }
      for (i = 1; i < K; i++)
      {
         int sz1 = maxp[t][i].size();
         for (j = 0; j < sz1; j++)
         {
            int x1 = maxp[t][i][j].k;
            int i0 = i - 1;
            if (i0 == 0) i0 = K - 1;

            double mx = Zero;
            trace[t][i][j].v = -1;

            int sz2 = maxp[t-1][i0].size();
            for (k = 0; k < sz2; k++)
            {
               int x0 = maxp[t-1][i0][k].k;  
               double tmp = maxp[t-1][i0][k].v + bigram[x0][x1];
               if (tmp > mx)
               {
                  mx = tmp;
                  trace[t][i][j].v = i0 * M + k;
               }
            }
            if (i == 1 && maxp[t-1][0].size() > 0)
            {
               double tmp = maxp[t-1][0][0].v + bigram[0][x1];
               if (tmp > mx)
               {
                  mx = tmp;
                  trace[t][i][j].v = 0;
               }
            }
            maxp[t][i][j].v = mx + encode[o][i][j].v;
         }
      }
   }

   double mx = Zero;
   int maxi, maxj;
   for (i = 0; i < K; i++)
   {
      int sz = maxp[n-1][i].size();
      for (j = 0; j < sz; j++)
      {
         if (maxp[n-1][i][j].v > mx)
         {
            mx = maxp[n-1][i][j].v;
            maxi = i;
            maxj = j;
         }
      }
   }

   for (t = n - 1; t >= 0; t--)
   {
      auto &tr = trace[t][maxi][maxj];
      out[t] = tr.k;
      maxi = tr.v / M;
      maxj = tr.v % M;
   }
}

void init()
{
   int t, i, j;

   for (t = 0; t < n; t++)
   {
      int o = inp[t];
      for (i = 0; i < K; i++)
      {
         if (t == 0 && i != 1) continue;
         int sz = encode[o][i].size();
         forward[t][i].resize(sz);
         backward[t][i].resize(sz);
         maxp[t][i].resize(sz);
         trace[t][i].resize(sz);
         for (j = 0; j < sz; j++)
            forward[t][i][j].k = backward[t][i][j].k = maxp[t][i][j].k = trace[t][i][j].k = encode[o][i][j].k;
      }
   }

   for (i = 0; i < M; i++)
   {
      pi[i] = -log(M);
      for (j = 0; j < M; j++)
         bigram[i][j] = -log(M);
   }
}

void load_model()
{
   FILE *f = fopen("em.model", "r");
   int i, j;
   int k, x, y;
   double p;
   for (i = 0; i < M; i++)
      fscanf(f, "%lf", &pi[i]);
   for (i = 0; i < M; i++)
      for (j = 0; j < M; j++)
         fscanf(f, "%lf", &bigram[i][j]);
   while (~fscanf(f, "%d %d %d %lf", &k, &x, &y, &p))
      encode[y][k].push_back({ x, p });
   fclose(f);
}

void save_model()
{
   FILE *f = fopen("em.model", "w");
   int i, j;

   for (i = 0; i < M; i++)
      fprintf(f, "%lf ", pi[i]);
   fprintf(f, "\n");
   for (i = 0; i < M; i++)
      for (j = 0; j < M; j++)
         fprintf(f, "%lf ", bigram[i][j]);
   fprintf(f, "\n");
   for (i = 0; i < M; i++)
      for (j = 0; j < K; j++)
         for (auto &x : encode[i][j])
            fprintf(f, "%d %d %d %lf\n", j, x.k, i, x.v);
   fclose(f);
}

void logg(int v)
{
   char fname[32];
   sprintf(fname, "out%d", v);
   FILE*f = fopen(fname, "w");
   viterbi();
   for (int i = 0; i < n; i++)
      fprintf(f, "%c", n2c(out[i]));
   fclose(f);
}

int main()
{
   int k, x, y;
   int i, j;
   FILE *f;
   int cnt[M][K] = {0};

   f = fopen("encode.table", "r");
   while (~fscanf(f, "%d %d %d", &k, &x, &y))
   {
      encode[y][k].push_back({ x, 0 });
      cnt[x][k]++;
   }
   fclose(f);
   for (i = 0; i < M; i++)
      for (j = 0; j < K; j++)
         for (auto &x : encode[i][j])
            x.v = -log(cnt[x.k][j]);

   char ch;
   f = fopen("test.txt", "r");
   n = 0;
   while ((ch = fgetc(f)) != EOF)
      inp[n++] = c2n(ch);
   fclose(f);

//   load_model();
   init();

   for (i = 0; i < 500; i++)
   {
      fprintf(stderr, "[%3d] ", i);
      EM_iter(n);
      if (i % 5 == 0)
      {
        logg(i);
        save_model();
      }
   }
   logg(i);
   save_model();

   return 0;
}
