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
  dataset = d;
}

BanditForest::~BanditForest() {

}


void BanditForest::SaveResults_Gain(const char *nomfichier)
// Build the result file
// format: t \t mean sum of rewards - CI \t mean sum of rewards \t mean sum of rewards + CI
{
	int t,n,i;
	float Regret,Gain,TC;
  unique_ptr<int[]> r(new int[M]), g(new int[M]);
  unique_ptr<float[]> c(new float[M]);
	FILE *f[1024];
	FILE *fic;
	char buf[1024];
	float classif_s,r_s,s;
	
	for (n=0;n<DRAWS;n++) {
		sprintf(buf,"%s.csv%d",nomfichier,n);
		f[n]=fopen(buf,"r"); 
	}
	sprintf(buf,"%s.csv",nomfichier);
	fic=fopen(buf,"w"); 
	fprintf(fic,"Step;R-;R;R+;G-;G;G+;A-;A;A+\n");
	
	for (t=TIME_DISPLAY;t<T;t=t+TIME_DISPLAY) {
		Regret=0.0;Gain=0.0;classif_s=0.0;r_s=0.0;s=0.0;TC=0.0;
		for (n=0;n<DRAWS;n++) {
      if (fscanf(f[n],"%d;%d;%d;%f",&t,&r[n],&g[n],&c[n]) == -1) {
        cerr << "fscanf" << endl;
        exit(EXIT_FAILURE);
      }
      Regret=Regret+ (float)r[n];
			Gain=Gain+g[n];
			TC=TC+c[n];
		}
    Regret /= (float)DRAWS;
		Gain=Gain/(float)DRAWS;
		TC=TC/(float)DRAWS;
		for(i=0;i<DRAWS;i++) {
      s=s+(float)(g[i]-Gain)*(g[i]-Gain);
      r_s = r_s + (float)(r[i] - Regret)*(r[i]-Regret);
      classif_s = classif_s + (float)(c[i] - TC)*(c[i]-TC);
    }

		s=sqrt(s/(float)(DRAWS));
    r_s=sqrt(r_s/(float)DRAWS);
    //printf("s : %f\n", s);
		fprintf(fic,"%d;%f;%f;%f;%f;%f;%f;%f;%f;%f\n", t, Regret - 1.96*r_s/sqrt(DRAWS), Regret, Regret + 1.96*r_s/sqrt(DRAWS), 
        Gain-1.96*s/(sqrt(DRAWS)), Gain, Gain+1.96*s/(sqrt(DRAWS)), 
        TC - 1.96*classif_s/sqrt(DRAWS), TC, TC + 1.96*classif_s/sqrt(DRAWS));
	}
	for (n=0;n<DRAWS;n++) {
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


void BanditForest::GetContext(uint pos, uint bruit)
// feed current_context with dataset[pos] and apply the bit inversion noise (bruit %)
{
	int i;

	for (i=0;i<M;i++) {
		if ((uint)RAND_R(100.0)>= bruit) x_courant[i] = dataset.context(pos, i);
		else x_courant[i] = 1-dataset.context(pos,i);
	}
}


int BanditForest::Decide_Vote(Tree **p)
// return the action selected by voting
{
  vector<int> Votes(K);
	for (int j=0;j<NB_TREE;j++) {
		Votes[p[j]->getAction()] += 1;
	}
  return Votes.size() - 1 - (max_element(Votes.rbegin(), Votes.rend()) - Votes.rbegin());

}


int BanditForest::PlayOLDP3(const char *nomfichier) {
	// KMD-Forest VE /AE Decide Vote
	int t,n,c,j;
	vector<bool> A;
	int Gain,Regret,TC;
	FILE *fic;
	char buf[1024];
	int reward,r_hat;
	Tree **path;

  Forest=(Tree **)malloc(sizeof(Tree *)*NB_TREE);
	path=(Tree **)malloc(sizeof(Tree *)*NB_TREE);
  A = vector<bool>(K, true);
	x_courant = vector<short>(M);

	for (n=0;n<DRAWS;n++) {
    cout << "--- Draw nb : " << n+1 << " ---" << endl;
		sprintf(buf,"%s.csv%d",nomfichier,n);
		fic=fopen(buf,"w");
		Gain=0;
    Regret=0;
    for (j=0;j<NB_TREE;j++) {
      Forest[j] = new Tree();
      Forest[j]->allocPath({(uint)M}, 0);
    }
		c=1;
    uint dataset_size = dataset.getShape().first;
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
			for (j=0;j<NB_TREE;j++) {
        // c0 = c0(xt)
				path[j]=Forest[j]->treeSearch(x_courant);
        // si d0 != D0 ....
        // si on est en VariableSelection et que l'arbre est à une profondeur maximale
				if ((t >= TL && path[j]->getState() == VARIABLE_SELECTION) || (path[j]->getDepth() == path[j]->getMaxDepth() && path[j]->getState() == VARIABLE_SELECTION)) {
          // cheminsTermines = 1
          path[j]->changeState(ACTION_ELIMINATION);
        }
        // si l'arbre n'est pas à la profondeur max, mais qu'on est en VariableSelection
				if (path[j]->getState() == VARIABLE_SELECTION) {
					path[j]->setAction(RoundRobin(path[j]->getAction(),A));
					//path[j]->setAction((path[j]->getAction() + 1) % K);
				}
        // sinon
				if (path[j]->getState() == ACTION_ELIMINATION) {
          path[j]->setAction(RoundRobin(path[j]->getAction(),path[j]->getAD()));
        }
        // Si cheminsTerminés
				if (path[j]->getState() == EXPLOIT) {
          path[j]->setAction(path[j]->getK());
        }
			}
      // Jouer le bras k et obtenir la récompense yk(t)
			a_current=Decide_Vote(path);
      bool to_continue = false;
      for (j=0; j < NB_TREE; j++) {
        if (path[j]->getState() == ACTION_ELIMINATION) {
          if (changePointDetectors.find(path[j]) != changePointDetectors.end() && changePointDetectors[path[j]]->update(t, dataset.best_arm(pos_current))) {
            for (auto &it : changePointDetectors) delete it.second;
            changePointDetectors.clear();
		        for (j=0;j<NB_TREE;j++) Forest[j]->freeKMD();
            for (j=0;j<NB_TREE;j++) {
              Forest[j] = new Tree();
              Forest[j]->allocPath({(uint)M}, 0);
            }
            to_continue = true;
            break;
          }
        }
      }
      if (to_continue) {
        continue;
      }
			reward=(dataset.best_arm(pos_current)==a_current);
			r_hat=reward;
      // Pour chaque 0 faire
			for (j=0;j<NB_TREE;j++) {
        // t = t+1 on incrémente le temps local du noeud
				if (path[j]->getState() == VARIABLE_SELECTION) {
          path[j]->updatePath(reward, a_current, x_courant);
          // Sinon
          if (a_current == K-1) {
            // S = VE
            path[j]->treeBuild();
          }
        }
        // si c0 = D0
				if (path[j]->getState() == ACTION_ELIMINATION) {
          // S = AE
				  if (path[j]->getAD()[a_current]==1) {
            path[j]->updateLeaf(r_hat,a_current);
          }
          // Dans AE : Si k = Dernier(St)
					if (a_current == path[j]->lastAction()) {
            path[j]->actionElimination();
            if (changePointDetectors.size() < 3 && path[j]->getState() == EXPLOIT) {
              changePointDetectors[path[j]] = new RBOCPD(T-t, path[j]->getK());
            }
          }
				}
			}

      TC += reward;
      Gain += reward;
      Regret += 1 - reward;

			if (c == TIME_DISPLAY) {
        fprintf(fic,"%d;%d;%d;%f\n",t+1,Regret,Gain,TC/(float)TIME_DISPLAY);
        c=1;
        TC=0;
			}
			else c++;
			pos_current++;
			if (pos_current == (uint)CONTEXTS_NB) pos_current=0;
			t += 1;
      if (t == 2000005) {
        dataset.permuteClasses(2);
      }
		}
		fclose(fic);
		for (j=0;j<NB_TREE;j++) Forest[j]->freeKMD();
    for (auto &it : changePointDetectors) delete it.second;
    changePointDetectors.clear();
	}

	SaveResults_Gain(nomfichier);
	free(Forest);
	free(path);

	return 1;
}
