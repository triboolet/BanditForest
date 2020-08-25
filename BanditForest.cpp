#include <iostream>
#include <math.h>
// unique_ptr
#include <memory>
#include <algorithm>
#include "consts.hpp"
#include "BanditForest.hpp"
#include "Tree.hpp"
#include "utils.hpp"

using namespace std;

BanditForest::BanditForest(Dataset d) {
  x = d;
}

BanditForest::~BanditForest() {

}


void BanditForest::SaveResults_Gain(const char *nomfichier)
// Build the result file
// format: t \t mean sum of rewards - CI \t mean sum of rewards \t mean sum of rewards + CI
{
  cout << nomfichier << "in save";
	int t,n,i;
	float Regret,Gain,TC;
  unique_ptr<int[]> r(new int[M]), g(new int[M]);
  unique_ptr<float[]> c(new float[M]);
	FILE *f[1024];
	FILE *fic;
	char buf[1024];
	float classif_s,r_s,s;
	
	for (n=0;n<Draws;n++) {
		sprintf(buf,"%s.csv%d",nomfichier,n);
		f[n]=fopen(buf,"r"); 
	}
	sprintf(buf,"%s.csv",nomfichier);
	fic=fopen(buf,"w"); 
	fprintf(fic,"Step;R-;R;R+;G-;G;G+;A-;A;A+\n");
	
	for (t=TimeDisplay;t<T;t=t+TimeDisplay) {
		Regret=0.0;Gain=0.0;classif_s=0.0;r_s=0.0;s=0.0;TC=0.0;
		for (n=0;n<Draws;n++) {
      if (fscanf(f[n],"%d;%d;%d;%f",&t,&r[n],&g[n],&c[n]) == -1) {
        cerr << "fscanf" << endl;
        exit(EXIT_FAILURE);
      }
      Regret=Regret+ (float)r[n];
			Gain=Gain+g[n];
			TC=TC+c[n];
		}
    Regret /= (float)Draws;
		Gain=Gain/(float)Draws;
		TC=TC/(float)Draws;
		for(i=0;i<Draws;i++) {
      s=s+(float)(g[i]-Gain)*(g[i]-Gain);
      r_s = r_s + (float)(r[i] - Regret)*(r[i]-Regret);
      classif_s = classif_s + (float)(c[i] - TC)*(c[i]-TC);
    }

		s=sqrt(s/(float)(Draws));
    r_s=sqrt(r_s/(float)Draws);
    //printf("s : %f\n", s);
		fprintf(fic,"%d;%f;%f;%f;%f;%f;%f;%f;%f;%f\n", t, Regret - 1.96*r_s/sqrt(Draws), Regret, Regret + 1.96*r_s/sqrt(Draws), 
        Gain-1.96*s/(sqrt(Draws)), Gain, Gain+1.96*s/(sqrt(Draws)), 
        TC - 1.96*classif_s/sqrt(Draws), TC, TC + 1.96*classif_s/sqrt(Draws));
	}
  cout << "okay writing";
	for (n=0;n<Draws;n++) {
		fclose(f[n]);
		sprintf(buf,"%s.csv%d",nomfichier,n);
		remove(buf);
	}
	fclose(fic);
}


int BanditForest::RoundRobin(int k, const vector<bool>& A)
// Play the next remaining action in A
{
	int i=k+1;
	
	if (i >= K) i=0;
	
	while (i<K && A[i] != 1) {
		i++;
		if (i==K) i=0;
	}

	return i;
}


void BanditForest::PrintContext()
// Print the current context oracle
{
	int i;
	
	for (i=0;i<M;i++) cout << x_courant[i] << "_"; 
  //cout << "k=" << oracle[pos_current] << endl;
}

void BanditForest::GetContext(uint pos, uint bruit)
// feed x_courant with x[pos] and apply the bit inversion noise (bruit %)
{
	int i;

	for (i=0;i<M;i++) {
		if ((uint)RAND_R(100.0)>= bruit) x_courant[i]=x.context(pos, i);
		else x_courant[i]=1-x.context(pos,i);
	}
}


int BanditForest::Decide_Vote(Tree **p)
// return the action selected by voting
{
  vector<int> Votes(K);
	for (int j=0;j<nbTree;j++) {
		Votes[p[j]->getAction()] += 1;
	}
  return Votes.size() - 1 - (max_element(Votes.rbegin(), Votes.rend()) - Votes.rbegin());

}


int BanditForest::PlayOLDP3(const char *nomfichier) {
  cout << nomfichier << "in P3" << endl;
	// KMD-Forest VE /AE Decide Vote
	int t,n,c,j;
	vector<bool> A;
	int Gain,Regret,TC;
	FILE *fic;
	char buf[1024];
	int reward,r_hat;
	Tree **path;

  Forest=(Tree **)malloc(sizeof(Tree *)*nbTree);
	path=(Tree **)malloc(sizeof(Tree *)*nbTree);
  A = vector<bool>(K, true);
	x_courant = make_unique<short[]>(M);

	for (n=0;n<Draws;n++) {
    cout << "--- Draw nb : " << n+1 << " ---" << endl;
		sprintf(buf,"%s.csv%d",nomfichier,n);
		fic=fopen(buf,"w");
		Gain=0;
    Regret=0;
    for (j=0;j<nbTree;j++) {
      Forest[j] = new Tree();
      Forest[j]->AllocPath({(uint)M}, 0);
    }
		c=1;
    uint dataset_size = x.getShape().first;
		pos_current=(uint) RAND_R(dataset_size);
		a_current=0;
		t=0;
		TC=0;

		while (t<T) {
      if (t > 0 && t % (T/10) == 0) 
        cout << "\tIteration " << t << " out of " << T << endl;
      // Observer le vecteur de contexte xt
			GetContext(pos_current,NOISE);
      // Pour chaque 0 faire
			for (j=0;j<nbTree;j++) {
        // c0 = c0(xt)
				path[j]=Forest[j]->TreeSearch(x_courant);
        // si d0 != D0 ....
        // si on est en VariableSelection et que l'arbre est à une profondeur maximale
				if ((t >= TL && path[j]->getState() == 0) || (path[j]->getDepth() == path[j]->getMaxDepth() && path[j]->getState() == 0)) {
          // cheminsTermines = 1
          path[j]->ChangeState(ACTION_ELIMINATION);
        }
        // si l'arbre n'est pas à la profondeur max, mais qu'on est en VariableSelection
				if (path[j]->getState() == 0) {
					path[j]->setAction(RoundRobin(path[j]->getAction(),A));
					//path[j]->setAction((path[j]->getAction() + 1) % K);
				}
        // sinon
				if (path[j]->getState() == 1) {
          path[j]->setAction(RoundRobin(path[j]->getAction(),path[j]->getAD()));
        }
        // Si cheminsTerminés
				if (path[j]->getState() == 2) {
          path[j]->setAction(path[j]->getK());
        }
			}
      // Jouer le bras k et obtenir la récompense yk(t)
			a_current=Decide_Vote(path);
      bool to_continue = false;
      for (j=0; j < nbTree; j++) {
        if (path[j]->getState() == 2) {
          if (changePointDetectors.find(path[j]) != changePointDetectors.end() && changePointDetectors[path[j]]->update(t, x.best_arm(pos_current))) {
            cout << "changed !" << endl;
            for (auto &it : changePointDetectors) delete it.second;
            changePointDetectors.clear();
		        for (j=0;j<nbTree;j++) Forest[j]->FreeKMD();
            for (j=0;j<nbTree;j++) {
              Forest[j] = new Tree();
              Forest[j]->AllocPath({(uint)M}, 0);
            }
            to_continue = true;
            break;
          }
        }
      }
      if (to_continue) {
        continue;
      }
			reward=(x.best_arm(pos_current)==a_current);
			r_hat=reward;
      // Pour chaque 0 faire
			for (j=0;j<nbTree;j++) {
        // t = t+1 on incrémente le temps local du noeud
				if (path[j]->getState() == 0) {
          path[j]->UpdatePath(reward, a_current, x_courant);
          // Sinon
          if (a_current == K-1) {
            // S = VE
            path[j]->TreeBuild();
          }
        }
        // si c0 = D0
				if (path[j]->getState() == 1) {
          // S = AE
				  if (path[j]->getAD()[a_current]==1) {
            path[j]->UpdateLeaf(r_hat,a_current);
          }
          // Dans AE : Si k = Dernier(St)
					if (a_current == path[j]->LastAction()) {
            path[j]->ActionElimination();
            if (changePointDetectors.size() < 3 && path[j]->getState() == 2) {
              changePointDetectors[path[j]] = new RBOCPD(T-t, path[j]->getK());
            }
          }
				}
			}

      TC += reward;
      Gain += reward;
      Regret += 1 - reward;

			if (c == TimeDisplay) {
        fprintf(fic,"%d;%d;%d;%f\n",t+1,Regret,Gain,TC/(float)TimeDisplay);
        c=1;
        TC=0;
			}
			else c++;
			pos_current++;
			if (pos_current == (uint)CONTEXTS_NB) pos_current=0;
			t += 1;
      if (t == 2000005) {
        x.permuteClasses(2);
        cout << "Permuted" << endl;
      }
		}
		fclose(fic);
		for (j=0;j<nbTree;j++) Forest[j]->FreeKMD();
    for (auto &it : changePointDetectors) delete it.second;
    changePointDetectors.clear();
	}

	SaveResults_Gain(nomfichier);
	free(Forest);
	free(path);

	return 1;
}
