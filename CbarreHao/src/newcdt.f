C=======================================================================
C BEGINNING OF FILE CDT.FOR
C=======================================================================
C VERSION DECEMBER 2, 1994
C=======================================================================
      SUBROUTINE CDT(N,ORDX,X,MAXND,INF,ALPHA,ZEUR,ZSTAR,FSTAR,LB0,LBC,
     +               NEXP,NPROBQ,NASS,ACTIVE,LOPT,SPARS,AVSON,ERR)
C
C SUBROUTINE CDT FOR THE SOLUTION OF THE
C        **** ASYMMETRIC TRAVELLING SALESMAN PROBLEMS  ****
C
C WRITTEN ACCORDING TO THE PAPER:
C
C "A BRANCH-AND-BOUND ALGORITHM FOR LARGE SCALE ASYMMETRIC TRAVELLING
C  SALESMAN PROBLEMS", BY G.CARPANETO, M.DELL'AMICO AND P.TOTH
C  TECH. REPORT DIPARTIMENTO DI ECONOMIA POLITICA
C  FACOLTA' DI ECONOMIA E COMMERCIO, UNIVERSITA' DI MODENA, 1990, ITALY
C
C I/O PARAMETER --------------------------------------------------------
C
C INPUT PARAMETERS
C
C   ALPHA   =  IF .GT. 0 DEFINE THE ARTIFICIAL UPPER BOUND UB=ALPHA*LB0
C              IF .LE. 0 NO EFFECT
C   N       =  NUMBER OF NODES
C   INF     =  VERY LARGE NUMBER
C   MAXND   =  IF GREATER OR EQUAL TO ZERO IT IS THE MAXIMUM NUMBER OF
C              NODE TO BE EXPLORED ON THE BRANCH DECISION-TREE
C   ORDX    =  DIMENSION OF VECTOR X
C   ZEUR    =  IF GREATER THEN ZERO IT MUST DEFINE A VALID UPPER BOUND
C
C   THE COST MATRIX IS STORED (COLUMN BY COLUMN) IN THE FIRST N**2 ELEM.
C   OF THE VECTOR X. THE COST MATRIX IT IS NOT AVAILABLE IN OUTPUT.
C
C OUTPUT VALUE
C
C   ACTIVE  = NUMBER OF ACTIVE PROBLEMS IN QUEUE WHEN THE PROGRAM STOPS
C             (NOT YET AND NOT CURRENTLY EXAMINED)
C   AVSON   = AVERAGE NUMBER OF SON NODES FOR THE EXPLORED PROBLEMS
C   ERR     =  .GT. ZERO IF AN ERROR OCCURRED
C           =  -1 IF NO SOLUTION WITH VALUE LESS THAN THE ARTIFICIAL
C              UPPER BOUND WAS FOUND.
C              IN THIS CASE ZSTAR IS THE ARTIFICIAL UPPER BOUND VALUE
C   FSTAR(I)=  SUCCESSOR OF NODE I IN THE CURRENT BEST SOLUTION
C   LB0     =  VALUE OF THE OPTIMAL ASSIGNMENT AT THE ROOT NODE
C   LBC     =  VALUE OF THE CURRENT BEST LB, I.E. VALUE OF THE LOWER
C              BOUND OF THE LAST PROBLEM EXTRACTED FROM THE QUEUE;
C              LBC IS A VALID LOWER BOUND FOR THE INSTANCE
C   NASS    =  NUMBER OF ASSIGNMENT SOLVED
C   NEXP    =  NUMBER OF EXPLORED PROBLEMS
C   NPROBQ  =  NUMBER OF PROBLEMS STORED IN THE QUEUE
C   SPARS   =  SPARSITY OF THE REDUCED MATRIX
C   ZEUR    =  MINIMUM VALUE BETWEEN THE VALUE OF ZEUR IN INPUT AND THE
C              VALUE OF THE APPROXIMATE SOLUTION COMPUTED USING THE
C              PATCHING HEURISTIC
C   ZSTAR   =  VALUE OF THE CURRENT BEST SOLUTION
C
C INTERNAL STORAGE OF  VECTORS AND MATRICES  IN VECTOR X
C
C   X(MM1)  =  THE QUEUE V(ORDV) (DINAMICALLY ALLOCATED)
C   X(MM2)  =  FIRST ELEMENT OF MATRICES MV,MF (DINAMICALLY ALLOCATED)
C   X(MM3)  =  VECTOR IC(*)
C   X(MM4)  =  VECTORS C(*) (CRA(*) IN CTCS,
C   X(MM5)  =                ICA(*) IN CTCS)
C   X(MM6)  =  IVA(N)
C   X(MM7)  =  IVB(N)
C   X(MM8)  =  CR(N+1)
C   X(MM9)  =  PRIMAL SOLUTION OF THE MAP ASSOCIATED WITH THE CURRENT
C              BRANCHED NODE F(N)
C   X(MM10) =  DUAL VAR. OF THE COLUMNS IN THE SOLUTION OF AP, DUALV(N)
C   X(MM11) =  PRIMAL SOLUTION OF THE MAP AT THE CURRENT NODE FC(N)
C   X(MM12) =  CURRENT DUAL VARIABLES OF MAP, VD(N)
C   X(MM13) =  THE HEURISTIC SOLUTION OF ATSP
C   X(MM14)..X(MM26) = TEMPORARY VECTORS
C
C   FMVF = POINTER TO THE FIRST FREE ELEMENT TO STORE COLUMNS OF
C          MATRICES MV,MF
C   X(FMVF) = POINTER TO THE NEXT FREE ELEMENT FOR MATRICES MV,MF
C
C LOCAL VARIABLES
C
C     .. Scalar Arguments ..
      REAL ALPHA,AVSON,SPARS
      INTEGER ACTIVE,ERR,INF,LB0,LBC,LOPT,MAXND,N,NASS,NEXP,NPROBQ,ORDX,
     +        ZEUR,ZSTAR
C     ..
C     .. Array Arguments ..
      INTEGER FSTAR(N),X(ORDX)
C     ..
C     .. Local Scalars ..
      INTEGER FMVF,I,IBIG,IDELTA,IENLRG,IMP,INF2,IPP,ISALVA,
     +        ISALVI,IVAI,K,MAXDIX,MAXICA,MM1,MM10,MM11,MM12,MM13,MM14,
     +        MM15,MM16,MM17,MM18,MM19,MM2,MM20,MM21,MM22,MM23,MM24,
     +        MM25,MM26,MM3,MM4,MM5,MM6,MM7,MM8,MM9,NC,NCI,NCODAL,NGEN,
     +        NLSP,NNODI,NNODIN,NODOIM,NPROB,NPROBV,NQ,OFFV,ORCR,ORCRA,
     +        ORDSP,ORDV,PNUOVO,PSALVO,PUNTA,PUNTB,PUNTLV,R,SC1,TOTASS,
     +        UR,VIMPA,VIMPB,Z,ZARF,ZC,ZERI
      LOGICAL ARTIF,EUR
C     ..
C     .. External Functions ..
C     ..
C     .. External Subroutines ..
      EXTERNAL ACTPRO,AGMHP,CALCUD,CALCUR,CERCSB,CLEARQ,CONTCI,COPYX,
     +         CREAMS,CTCS,ENLARG,ENLINI,ERRORS,EXQUE,GENSON,INQUE,
     +         INQUER,KARP
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT,INT
C     ..
      INF2 = FLOAT(INF)/2.
      NQ = N**2
      ERR = 0
C
      MM26 = ORDX - N + 1
      MM25 = MM26 - N
      MM24 = MM25 - N
      MM23 = MM24 - N
      MM22 = MM23 - N
      MM21 = MM22 - N
      MM20 = MM21 - N
      MM19 = MM20 - N
      MM18 = MM19 - N
      MM17 = MM18 - N
      MM16 = MM17 - N
      MM15 = MM16 - N
      MM14 = MM15 - N
      MM13 = MM14 - N
      MM12 = MM13 - N
      MM11 = MM12 - N
      MM10 = MM11 - N
      MM9 = MM10 - N
      MM8 = MM9 - N - 1
      MM7 = MM8 - N
      MM6 = MM7 - N
      IF (MM6.GT.NQ) THEN
          MM5 = NQ + (MM6-NQ)*2./3.
          MM4 = NQ + (MM6-NQ)/3.
          MM3 = NQ + 1
          MAXICA = MM6 - MM5 + 1
          ORCRA = MM5 - MM4 + 1
          MAXDIX = MM4 - MM3 + 1
          ORCR = N + 1
C
          NPROB = 1
          PSALVO = 0
          NCODAL = 0
          PUNTLV = 1
          VIMPA = 0
          VIMPB = 0
          SC1 = -1
          NGEN = 1
          FMVF = 0
          NASS = 1
C
C STATISTICS INFORMATION
C
          NEXP = 1
          NPROBQ = 1
          LOPT = 1
          AVSON = 0.
          SPARS = 0.
          ACTIVE = 0
C
C   SOLVE THE INITIAL ASSIGNMENT PROBLEM
C
          CALL CTCS(N,Z,1,X(1),X(MM9),X(MM13),X(MM14),X(MM15),X(MM16),
     +              X(MM17),X(MM4),X(MM5),X(MM8),X(MM3),X(MM26),X(MM10),
     +              X(MM20),X(MM21),X(MM23),X(MM22),X(MM24),MAXDIX,
     +              MAXICA,X(MM25),ORCR,ORCRA,INF2,ERR)
          IF (ERR.EQ.0) THEN
              LB0 = Z
              LBC = Z
C
C   TOTASS STORES THE VALUE OF THE AP SOLUTION AT THE ROOT NODE
C
              TOTASS = Z
              CALL CONTCI(X(MM9),NCI,N,X(MM14))
              CALL ENLINI(N,X(MM8),X(MM3),X(MM9),X(MM26),X(MM10),
     +                    X(MM14),X(MM15),X(MM16),X(MM17),X(1),X(MM4),
     +                    X(MM5),NC,INF2)
              ZSTAR = TOTASS
              IF (NC.EQ.1) THEN
C
C  OPTIMAL SOLUTION FOUND AT THE ROOT NODE BY CTCS OR ENLINI
C
                  CALL COPYX(X(MM9),FSTAR,N)
                  ZSTAR = TOTASS
                  RETURN
              ELSE
C
                  CALL COPYX(X(MM9),X(MM13),N)
                  CALL KARP(N,X(1),X(MM13),X(MM15),X(MM16),X(MM17),
     +                      X(MM18),TOTASS,ZSTAR,INF2)
C     CHECK IF THE HEURISTIC SOLUTION IS AN OPTIMUM
                  IF (ZEUR.GT.0 .AND. ZEUR.LT.ZSTAR) THEN
                      EUR = .TRUE.
                      ZSTAR = ZEUR
                  ELSE
                      EUR = .FALSE.
                      CALL COPYX(X(MM13),FSTAR,N)
                  END IF
                  IF (ZSTAR.EQ.TOTASS) RETURN
                  AVSON = NC
                  ARTIF = .FALSE.
                  IF (ALPHA.GT.0.0) THEN
                      IF (TOTASS*ALPHA.LT.ZSTAR) THEN
                          ZSTAR = TOTASS*ALPHA
                          ZARF = ZSTAR
                          ARTIF = .TRUE.
                      END IF
                  END IF
C
                  IDELTA = ZSTAR - LB0
                  CALL CREAMS(IDELTA,X(1),N,X(MM8),X(1),MM6,MM4,MM3,
     +                        X(MM26),X(MM10),ZERI,NLSP,ERR)
                  IF (ERR.EQ.0) THEN
                      ORDSP = NLSP
                      SPARS = NLSP*100./FLOAT(N* (N-1))
                      IENLRG = 0
                      IF (FLOAT(ZERI).GT.2.5*FLOAT(N)) IENLRG = 1
C
C   CHOOSE THE SUBTOUR FOR THE BRANCH PHASE
C
                      CALL CERCSB(X(MM9),X(MM14),X(MM10),N,NNODI,ISALVA,
     +                            INF2)
                      NNODIN = NNODI
                      ISALVI = ISALVA
C
C   DEFINE THE WORKING ARRAYS TO STORE THE QUEUE:
C   SCALAR AND VECTORIAL INFORMATION
C
                      MM1 = 1
                      MM2 = MM3
                      ORDV = MM2 - MM1
C
C MM1 = FIRST ELEMENT CONTAINING SCALAR INFORMATIONS
C MM2 = FIRST POSITION OCCUPIED BY  MATRICES MF,MV
C
C
C    INSERT THE ROOT NODE IN QUEUE AND INITIALIZE THE VECTORS
C    FOR THE BRANCHING PHASE
C
                      PUNTLV = 1
                      CALL INQUER(ISALVI,NNODIN,PSALVO,NGEN,N,TOTASS,
     +                            VIMPA,VIMPB,X(MM9),X(MM10),PUNTLV,
     +                            PUNTA,PUNTB,ORDV,X(MM6),X(MM7),X(MM1),
     +                            PNUOVO,NPROB,SC1,OFFV,ERR)
                      IF (ERR.EQ.0) THEN
                          PSALVO = PNUOVO
                          ACTIVE = 0
C                          WRITE (6,FMT=
C     +                      '('' ROOT NODE: ZSTAR='',I10,'' LB0='',I10)'
C     +                      ) ZSTAR,LB0
                      ELSE
                          CALL ERRORS(ERR,4)
                          RETURN
                      END IF
                  ELSE
                      CALL ERRORS(ERR,3)
                      RETURN
                  END IF
              END IF
          ELSE
              CALL ERRORS(ERR,2)
              RETURN
          END IF
      ELSE
          CALL ERRORS(ERR,1)
          RETURN
      END IF
C
C PSALVO IS THE POINTER TO THE ACTUAL PROBLEM
C
  100 DO 200 I = 1,NPROB
C
C  GENERATE THE DESCENDING NODES OF PSALVO
C
          CALL GENSON(N,X(MM6),X(MM7),X(MM10),NPROB,I,VIMPA,VIMPB,
     +                NODOIM,X(MM8),ORDSP,X(MM3),X(MM4),INF2)
          CALL COPYX(X(MM9),X(MM11),N)
          CALL COPYX(X(MM10),X(MM12),N)
          ZC = TOTASS
          IVAI = X(MM6+I-1)
C
C   SOLVE THE NEW MAP PROBLEM
C
          CALL CALCUR(N,X(MM8),ORDSP,X(MM3),X(MM4),X(MM11),IVAI,X(MM12),
     +                UR,INF2)
          CALL AGMHP(N,IVAI,X(MM8),ORDSP,X(MM3),X(MM4),X(MM11),X(MM12),
     +               UR,ZC,X(MM14),X(MM15),X(MM16),X(MM17),X(MM18),
     +               ZSTAR,IMP,INF2)
          IF (NASS.EQ.MAXND) THEN
              CALL ERRORS(ERR,-1)
              RETURN
          ELSE IF (IMP.NE.1) THEN
              IF (ZC.LT.ZSTAR) THEN
                  NASS = NASS + 1
C
C   COUNT THE SUBTOURS
C
                  CALL CONTCI(X(MM11),NC,N,X(MM15))
                  IF (IENLRG.NE.0) THEN
                      CALL CALCUD(N,ORDSP,X(MM3),X(MM8),X(MM4),X(MM12),
     +                            X(MM11),X(MM14))
                      CALL ENLARG(N,X(MM8),ORDSP,X(MM3),X(MM4),X(MM11),
     +                            X(MM14),X(MM12),X(MM15),X(MM16),
     +                            X(MM17),X(MM18),NC,INF2)
                  END IF
                  IF (NC.EQ.1) THEN
C
C  FOUND A NEW FEASIBLE SOLUTION
C
                      ZSTAR = ZC
C
C   CLEAR THE QUEUE OF THE ACTIVE PROBLEMS
C
                      CALL CLEARQ(ZSTAR,PUNTB,FMVF,ORDX,X(1),NCODAL,
     +                            ORDV,X(MM1),ACTIVE)
                      CALL COPYX(X(MM11),FSTAR,N)
C                      WRITE (6,FMT='('' ZSTAR='',I10,'' LBC='',I10,
C     +                      '' NPROBQ='',I8,'' ACTIVE='',I8)')
C     +                      ZSTAR,LBC,NPROBQ,ACTIVE
C STATISTICS
                      LOPT = 2
                      IPP = PSALVO
  105                 IPP = X(MM1+IPP+2)
                      LOPT = LOPT + 1
                      IF (IPP.GT.1) GO TO 105
C
C
C   IF THE COST ZC OF THE SOLUTION OF THE CURRENT PROBLEM P IS UGUAL TO
C   THE LB OF THE PROBLEM FATHER OF P, SAY PF,  NO OTHER SON OF PF
C   HAVE TO BE GENERATED
C
                      IBIG = X(MM1+PSALVO+1)
                      IF (ZC.LE.IBIG) THEN
                          NPROBV = I
                          GO TO 300
                      END IF
                  ELSE
                      CALL CERCSB(X(MM11),X(MM14),X(MM12),N,NNODI,
     +                            ISALVA,INF2)
                      NGEN = I
C
C  INSERT THE PROBLEM IN QUEUE
C
                      IF (FMVF.NE.0) THEN
                          K = FMVF
                          FMVF = X(FMVF)
                      ELSE
                          MM2 = MM2 - 2*N
                          K = MM2
                      END IF
                      ORDV = MM2 - MM1
                      IF (MM1+PUNTLV+OFFV+NNODI.LT.MM2) THEN
                          CALL INQUE(ISALVA,NNODI,SC1,PSALVO,NGEN,
     +                               X(MM12),X(MM11),N,ZC,X(K),X(K+N),K,
     +                               VIMPA,VIMPB,ORDV,X(MM1),NCODAL,
     +                               PUNTLV,PUNTA,PUNTB,OFFV,INF2)
                          NPROBQ = NPROBQ + 1
                          ACTIVE = ACTIVE + 1
                          R = INT(NPROBQ/1000.)*1000
C                          IF (R.EQ.NPROBQ) WRITE (6,FMT='('' ZSTAR='',
C     +                       I10,'' LBC='',I10,'' NPROBQ='',I8,
C     +                       '' ACTIVE='',I8)') ZSTAR,LBC,NPROBQ,ACTIVE
                      ELSE
                          CALL ERRORS(ERR,5)
                          RETURN
                      END IF
                  END IF
              END IF
          END IF
C
  200 CONTINUE
C
      NPROBV = NPROB
  300 IF (NCODAL.NE.0) THEN
C
C  EXTRACT THE PROBLEM WITH LOWEST BOUND
C
          CALL EXQUE(ORDV,X(MM1),ORDX,X(1),FMVF,X(MM10),X(MM9),
     +               PNUOVO,NGEN,PUNTA,NPROB,X(MM6),X(MM7),NCODAL,N,SC1,
     +               TOTASS,OFFV)
          AVSON = (AVSON*NEXP+NPROB)/FLOAT(NEXP+1)
          NEXP = NEXP + 1
          ACTIVE = ACTIVE - 1
          IF (LBC.LT.TOTASS) THEN
              LBC = TOTASS
C              WRITE (6,FMT='('' ZSTAR='',I10,'' LBC='',I10,'' NPROBQ='',
C     +               I8,'' ACTIVE='',I8)') ZSTAR,LBC,NPROBQ,ACTIVE
          END IF
          LBC = TOTASS
 
C
C   UPDATE SETS OF INCLUDED AND EXCLUDED ARCS
C
          CALL ACTPRO(PSALVO,N,NPROBV,PNUOVO,NGEN,VIMPA,VIMPB,ORDV,
     +                X(MM1),X(MM8),ORDSP,X(MM3),X(MM4),OFFV,INF2)
          PSALVO = PNUOVO
          GO TO 100
      END IF
C
C
C THE TREE SEARCH IS TERMINATED
C
      IF (ARTIF .AND. ZSTAR.EQ.ZARF) CALL ERRORS(ERR,6)
      IF (EUR) THEN
C          WRITE (6,FMT='('' ZEUR IS THE OPTIMAL SOLUTION VALUE'')')
C          WRITE (6,FMT='('' FSTAR() DO NOT CONTAIN THE OPTIMAL TOUR'')')
      END IF
      RETURN
      END
C
      SUBROUTINE CONTCI(F,NC,N,FLAG)
C     .. Scalar Arguments ..
      INTEGER N,NC
C     ..
C     .. Array Arguments ..
      INTEGER F(N),FLAG(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,K
C     ..
      NC = 0
      DO 100 I = 1,N
          FLAG(I) = 0
  100 CONTINUE
      DO 200 I = 1,N
          IF (FLAG(I).LE.0) THEN
              K = I
              NC = NC + 1
  120         FLAG(K) = 1
              K = F(K)
              IF (K.NE.I) GO TO 120
          END IF
  200 CONTINUE
      RETURN
      END
C
      SUBROUTINE LOADFV(F,V,N,VROOT,FROOT)
C     .. Scalar Arguments ..
      INTEGER N
C     ..
C     .. Array Arguments ..
      INTEGER F(N),FROOT(N),V(N),VROOT(N)
C     ..
C     .. Local Scalars ..
      INTEGER I
C     ..
      DO 100 I = 1,N
          FROOT(I) = F(I)
          VROOT(I) = V(I)
  100 CONTINUE
      RETURN
      END
C
      SUBROUTINE BACKFV(VROOT,FROOT,N,VD,F)
C     .. Scalar Arguments ..
      INTEGER N
C     ..
C     .. Array Arguments ..
      INTEGER F(N),FROOT(N),VD(N),VROOT(N)
C     ..
C     .. Local Scalars ..
      INTEGER I
C     ..
      DO 100 I = 1,N
          F(I) = FROOT(I)
          VD(I) = VROOT(I)
  100 CONTINUE
      RETURN
      END
C
      SUBROUTINE INQUER(ISALVA,NNODI,PSALVO,NGEN,N,TOTASS,VIMPA,VIMPB,F,
     +                  VD,PUNTLV,PUNTA,PUNTB,ORDV,IVA,IVB,V,P2P,NPROB,
     +                  SC1,OFFV,IERR)
C
C     .. Scalar Arguments ..
      INTEGER IERR,ISALVA,N,NGEN,NNODI,NPROB,OFFV,ORDV,P2P,PSALVO,PUNTA,
     +        PUNTB,PUNTLV,SC1,TOTASS,VIMPA,VIMPB
C     ..
C     .. Array Arguments ..
      INTEGER F(N),IVA(N),IVB(N),V(ORDV),VD(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,I32,KSALVA,NM1,NODOB,NODOPA,PLVP
C     ..
      I32 = 32000
      OFFV = 7
      IF (PUNTLV+OFFV+NNODI.LE.ORDV) THEN
          NM1 = NGEN - 1
          SC1 = SC1 + (NM1)*10 + 1
          V(PUNTLV+2) = TOTASS
          V(PUNTLV+3) = PSALVO
          V(PUNTLV+4) = NGEN*I32 + NNODI
          V(PUNTLV+5) = 1
          V(PUNTLV+6) = VIMPA*I32 + VIMPB
          V(PUNTLV+7) = SC1
          PLVP = PUNTLV + OFFV
          I = 1
          KSALVA = ISALVA
          NODOPA = ISALVA
   50     NODOB = F(NODOPA)
          V(PLVP+I) = NODOPA*I32 + NODOB
          IVA(I) = NODOPA
          IVB(I) = NODOB
          I = I + 1
          NODOPA = NODOB
          IF (NODOPA.NE.KSALVA) GO TO 50
          PUNTA = PUNTLV
          PUNTB = PUNTLV + 1
          PUNTLV = PUNTLV + OFFV + NNODI + 1
          P2P = PUNTA
          NPROB = NNODI
          SC1 = 0
          RETURN
      END IF
      IERR = 2
      RETURN
      END
C
      SUBROUTINE COPYX(F,FC,N)
C     .. Scalar Arguments ..
      INTEGER N
C     ..
C     .. Array Arguments ..
      INTEGER F(N),FC(N)
C     ..
C     .. Local Scalars ..
      INTEGER I
C     ..
      DO 100 I = 1,N
          FC(I) = F(I)
  100 CONTINUE
      RETURN
      END
C
      SUBROUTINE CTCS(N,Z,KS,A,F,R,VS,FS,UOLD,SUR,CRA,ICA,CR,IC,U,V,FB,
     +                PI,C,DM,LL,MAXDIX,MAXICA,RX,ORCR,ORCRA,INF,IERR)
C
C   SOLUTION OF THE LINEAR MIN-SUM ASSIGNMENT PROBLEM THROUGH THE
C   ALGORITHM PRESENTED IN THE PAPER:
C   G. CARPANETO AND P. TOTH, "PRIMAL-DUAL ALGORITHMS FOR THE
C   ASSIGNMENT PROBLEM", DISCRETE APPLIED MATHEMATICS 18, 137-153,1987
C
C     .. Scalar Arguments ..
      INTEGER IERR,INF,KS,MAXDIX,MAXICA,N,ORCR,ORCRA,Z
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),C(N),CR(ORCR),CRA(ORCRA),DM(N),F(N),FB(N),FS(N),
     +        IC(MAXDIX),ICA(MAXICA),LL(N),PI(N),R(N),RX(N),SUR(N),U(N),
     +        UOLD(N),V(N),VS(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,IFLAG,J,K,KFEAS,KOPT,KRIP,KTH,M,M1,MAX,MAXDIM,
     +        MAXFEA,MAXRIP,NFEAS,NKK,NUR,ZZ
C     ..
C     .. External Subroutines ..
      EXTERNAL APMMIX,ASMIXM,FEASO,INDUS3,OPTO,SETUPO
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT
C     ..
C     .. Data statements ..
      DATA MAXRIP/5/,MAXFEA/1/
C     ..
C
      MAXDIM = MAXDIX - N
      KRIP = 0
      NFEAS = 0
      DO 100 I = 1,N
          F(I) = 0
          FB(I) = 0
          CRA(I) = 0
          A(I,I) = INF
  100 CONTINUE
      NKK = 0
      CR(1) = -1
      M1 = -1
      CALL INDUS3(N,A,F,M,U,V,FB,PI,MAX,INF)
      Z = 0
      ZZ = 0
      DO 200 I = 1,N
          Z = Z + V(I)
          ZZ = ZZ + U(I)
  200 CONTINUE
      IF (M.NE.N) THEN
          IF (KS.EQ.1) THEN
C
              IF (FLOAT(M)/FLOAT(N).GT.0.6) THEN
                  IERR = 0
                  CALL SETUPO(N,A,U,V,M,IC,CR,KTH,MAXDIM,IFLAG,INF)
                  IF (IFLAG.NE.1) THEN
                      IF (CR(1).GE.0) THEN
                          DO 202 I = 1,N
                              VS(I) = V(I)
                              FS(I) = F(I)
                              UOLD(I) = U(I)
  202                     CONTINUE
  204                     CALL ASMIXM(N,A,IC,CR,F,FB,U,V,Z,PI,R,C,CRA,
     +                                ICA,SUR,NUR,DM,LL,RX,INF)
                          IF (IERR.NE.0) RETURN
                          IF (NUR.GT.0) THEN
                              IF (NFEAS.NE.MAXFEA) THEN
                                  NFEAS = NFEAS + 1
                                  CALL FEASO(N,A,KFEAS,KTH,UOLD,VS,
     +                                       NFEAS,CRA,ICA,NKK,SUR,NUR,
     +                                       MAXICA)
                                  IF (KFEAS.EQ.1) GO TO 204
                              END IF
                              DO 206 J = 1,N
                                  FB(J) = 0
  206                         CONTINUE
                              DO 208 I = 1,N
                                  J = FS(I)
                                  IF (J.GT.0) FB(J) = I
                                  U(I) = UOLD(I)
                                  V(I) = VS(I)
                                  F(I) = FS(I)
  208                         CONTINUE
                          ELSE
                              CALL OPTO(N,A,U,V,KOPT,CRA,ICA,NKK,F,FB,
     +                                  UOLD,MAXICA,IERR)
                              IF (IERR.EQ.0) THEN
                                  IF (KOPT.EQ.1) RETURN
                                  IF (KOPT.NE.M1) THEN
                                      IF (KRIP.NE.MAXRIP) THEN
                                          KRIP = KRIP + 1
                                          GO TO 204
                                      END IF
                                  END IF
                              END IF
                          END IF
                      END IF
                  END IF
              END IF
          END IF
          CALL APMMIX(N,A,F,Z,FB,U,V,PI,R,C,DM,LL,INF)
          RETURN
      END IF
      Z = 0
      DO 300 K = 1,N
          Z = Z + U(K) + V(K)
  300 CONTINUE
      RETURN
      END
C
      SUBROUTINE SETUPO(N,A,U,V,M,IC,CR,KTH,MAXDIM,IFLAG,INF)
C
C DEFINE THE SPARSE MATRIX CORRESPONDING TO THE COMPLETE COST MATRIX A.
C THE SPARSE MATRIX IS STORED THROUGH VECTORS IC,CR AND MATRIX A.
C
C     .. Scalar Arguments ..
      INTEGER IFLAG,INF,KTH,M,MAXDIM,N
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),CR(N+1),IC(*),U(N),V(N)
C     ..
C     .. Local Scalars ..
      REAL ALPHA,AM,DD,ISUM,PS,QQ,RA,SUM,TH
      INTEGER I,IA,INF2,IPS,J,JSTEP,L,LSTEP,NR
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC ALOG,FLOAT
C     ..
C     .. Data statements ..
      DATA QQ/2./
C     ..
      INF2 = FLOAT(INF)/2.
      IFLAG = 0
      LSTEP = 10
      JSTEP = N/LSTEP
      IF (N.LT.LSTEP) JSTEP = 1
      NR = (N+JSTEP-1)/JSTEP
      ISUM = 0.0
      DO 100 J = 1,N,JSTEP
          ISUM = ISUM - V(J)
  100 CONTINUE
      ISUM = ISUM*N
      DO 200 I = 1,N
          DO 150 J = 1,N,JSTEP
              IA = A(I,J)
              IF (IA.GT.INF2) THEN
                  ISUM = ISUM + U(I) + V(J)
              ELSE
                  ISUM = ISUM + IA
              END IF
  150     CONTINUE
          ISUM = ISUM - NR*U(I)
  200 CONTINUE
      SUM = ISUM
      DD = N*NR - NR
      ALPHA = SUM/DD
      AM = M
      RA = QQ*ALOG(FLOAT(N))/AM
      KTH = ALPHA*RA + 0.5
C
      L = 1
      DO 300 I = 1,N
          CR(I) = L
          TH = KTH + U(I)
          DO 250 J = 1,N
              IF (A(I,J)-V(J).LE.TH) THEN
                  IC(L) = J
                  L = L + 1
              END IF
  250     CONTINUE
          IF (L.GT.MAXDIM) GO TO 400
  300 CONTINUE
      CR(N+1) = L
      PS = 0.005
      IF (N.LE.950) PS = 0.01
      IF (N.LE.450) PS = 0.02
      IF (N.LE.250) PS = 0.03
      IPS = PS*FLOAT(N*N)
      IF (L.LT.IPS) IFLAG = 1
      RETURN
  400 CR(1) = -1
      RETURN
      END
C
      SUBROUTINE OPTO(N,A,U,V,KOPT,CRA,ICA,NKK,F,FB,UOLD,MAXICA,IERR)
C
C CHECK THE FEASIBILITY OF THE DUAL SOLUTION , AND HENCE THE
C OPTIMALITY OF THE PRIMAL SOLUTION FOUND BY THE SPARSE MATRIX
C PROCEDURE.
C
C     .. Scalar Arguments ..
      INTEGER IERR,KOPT,MAXICA,N,NKK
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),CRA(*),F(N),FB(N),ICA(MAXICA),U(N),UOLD(N),V(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,IA,J,JC,K,KK,KKN,MIN,NEG
C     ..
      KOPT = 0
      KK = NKK
      NEG = 0
      DO 100 I = 1,N
          IF (U(I).NE.UOLD(I)) THEN
              MIN = 0
              K = I
              DO 20 J = 1,N
                  IA = A(I,J) - U(I) - V(J)
                  IF (IA.LT.0) THEN
                      IF (IA.LT.0) NEG = NEG + 1
                      KK = KK + 1
                      IF (KK.LE.MAXICA) THEN
                          KKN = KK + N
    2                     IF (CRA(K).EQ.0) THEN
                              CRA(K) = KKN
                              CRA(KKN) = 0
                              ICA(KK) = J
                          ELSE
                              K = CRA(K)
                              GO TO 2
                          END IF
                      END IF
                      IF (IA.LT.MIN) MIN = IA
                  END IF
   20         CONTINUE
              U(I) = U(I) + MIN
              UOLD(I) = U(I)
              IF (MIN.NE.0) THEN
                  JC = F(I)
                  F(I) = 0
                  FB(JC) = 0
                  KOPT = KOPT - 1
              END IF
          END IF
  100 CONTINUE
      NKK = KK
      IF (NKK.LE.MAXICA) THEN
          IF (KOPT.LT.0) THEN
              KOPT = 0
              RETURN
          ELSE
              KOPT = 1
              RETURN
          END IF
      END IF
      IERR = 1
      KOPT = -1
      RETURN
      END
C
      SUBROUTINE ASMIXM(N,A,IC,CR,F,FB,DUALU,DUALV,Z,PRE,UV,SR,CRA,ICA,
     +                  SUR,NUR,DM,LL,R,INF)
C
C   SHORTEST AUGMENTING PATH AND HUNGARIAN METHOD :
C   VERSION FOR SPARSE MATRICES
C
C
C     .. Scalar Arguments ..
      INTEGER INF,N,NUR,Z
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),CR(*),CRA(*),DM(N),DUALU(N),DUALV(N),F(N),FB(N),
     +        IC(*),ICA(*),LL(N),PRE(N),R(N),SR(N),SUR(N),UV(N)
C     ..
C     .. Local Scalars ..
      INTEGER AA,D,I,I1,I2,I2P1,II,IIX,IK,IND,INDEX,INDEXV,IV,J,J1,K,K1,
     +        K2,KDIR,KMN,KNUV,LI,NUV,U,W
C     ..
      NUR = 0
      DO 500 U = 1,N
          IF (F(U).GT.0) GO TO 500
          DO 50 I = 1,N
              LL(I) = 0
              DM(I) = INF
   50     CONTINUE
          NUV = N
          J1 = 0
          KNUV = 0
          K1 = CR(U)
          K2 = CR(U+1) - 1
          DO 100 IK = K1,K2
              I = IC(IK)
              DM(I) = A(U,I) - DUALU(U) - DUALV(I)
              PRE(I) = U
              J1 = J1 + 1
              SR(J1) = I
  100     CONTINUE
          K = U
  150     IF (CRA(K).EQ.0) THEN
              R(1) = U
          ELSE
              K = CRA(K)
              KMN = K - N
              I = ICA(KMN)
              DM(I) = A(U,I) - DUALU(U) - DUALV(I)
              PRE(I) = U
              J1 = J1 + 1
              SR(J1) = I
              GO TO 150
          END IF
  200     D = INF
          INDEX = 0
          I2 = 1
          I1 = I2
C
          IF (NUV.LT.J1) THEN
              KDIR = 1
              IF (KNUV.NE.1) THEN
                  IV = 0
                  DO 210 I = 1,N
                      IF (LL(I).NE.1) THEN
                          IV = IV + 1
                          UV(IV) = I
                      END IF
  210             CONTINUE
                  KNUV = 1
              END IF
              DO 220 IV = 1,NUV
                  I = UV(IV)
                  IF (DM(I).LE.D) THEN
                      IF (DM(I).NE.D) THEN
                          INDEX = 0
                          I2 = I1
                      END IF
                      D = DM(I)
                      IF (FB(I).LE.0) THEN
                          INDEX = I
                          IF (D.EQ.0) GO TO 300
                      END IF
                      I2 = I2 + 1
                      R(I2) = IV
                  END IF
  220         CONTINUE
          ELSE
              KDIR = 0
C
              DO 240 LI = 1,J1
                  I = SR(LI)
                  IF (DM(I).LE.D) THEN
                      IF (LL(I).NE.1) THEN
                          IF (DM(I).NE.D) THEN
                              INDEX = 0
                              I2 = I1
                          END IF
                          D = DM(I)
                          IF (FB(I).LE.0) THEN
                              INDEX = I
                              IF (D.EQ.0) GO TO 300
                          END IF
                          I2 = I2 + 1
                          R(I2) = I
                      END IF
                  END IF
  240         CONTINUE
          END IF
          IF (D.EQ.INF) THEN
C
              NUR = NUR + 1
              SUR(NUR) = U
              GO TO 500
          ELSE IF (INDEX.LE.0) THEN
              I1 = I1 + 1
              I2P1 = I2 + I1
C
              DO 280 IIX = I1,I2
                  II = I2P1 - IIX
                  IF (KDIR.EQ.1) THEN
                      INDEXV = R(II)
                      INDEX = UV(INDEXV)
                      UV(INDEXV) = UV(NUV)
                  ELSE
                      INDEX = R(II)
                  END IF
                  LL(INDEX) = 1
                  NUV = NUV - 1
                  W = FB(INDEX)
                  K1 = CR(W)
                  K2 = CR(W+1) - 1
C
                  DO 250 IK = K1,K2
                      I = IC(IK)
                      IF (LL(I).NE.1) THEN
                          AA = D + A(W,I) - DUALU(W) - DUALV(I)
                          IF (DM(I).GT.AA) THEN
                              IF (DM(I).GE.INF) THEN
                                  J1 = J1 + 1
                                  SR(J1) = I
                              END IF
                              DM(I) = AA
                              PRE(I) = W
                          END IF
                      END IF
  250             CONTINUE
                  K = W
  260             IF (CRA(K).NE.0) THEN
                      K = CRA(K)
                      KMN = K - N
                      I = ICA(KMN)
                      IF (LL(I).NE.1) THEN
                          AA = D + A(W,I) - DUALU(W) - DUALV(I)
                          IF (DM(I).GT.AA) THEN
                              IF (DM(I).GE.INF) THEN
                                  J1 = J1 + 1
                                  SR(J1) = I
                              END IF
                              DM(I) = AA
                              PRE(I) = W
                          END IF
                      END IF
                      GO TO 260
                  END IF
  280         CONTINUE
              GO TO 200
          END IF
C
  300     DO 350 J = 1,N
              IF (DM(J).LT.D) THEN
                  DUALV(J) = DUALV(J) + DM(J) - D
                  I = FB(J)
                  DUALU(I) = DUALU(I) - DM(J) + D
              END IF
  350     CONTINUE
          DUALU(U) = DUALU(U) + D
C
  400     W = PRE(INDEX)
          FB(INDEX) = W
          IND = F(W)
          F(W) = INDEX
          IF (W.NE.U) THEN
              INDEX = IND
              GO TO 400
          END IF
  500 CONTINUE
      IF (NUR.LE.0) THEN
C
          Z = 0
          DO 550 I = 1,N
              Z = Z + DUALU(I) + DUALV(I)
  550     CONTINUE
          RETURN
      END IF
      Z = -1
      RETURN
      END
C
      SUBROUTINE FEASO(N,A,KFEAS,KTH,US,VS,NFEAS,CRA,ICA,NKK,SUR,NUR,
     +                 MAXICA)
C
C INSERT NEW ENTRIES IN ROWS SUR (L) (L = 1,NUR) IN ORDER TO FIND
C A FEASIBLE ASSIGNMENT IN THE SPARSE COST MATRIX.
C
C     .. Scalar Arguments ..
      INTEGER KFEAS,KTH,MAXICA,N,NFEAS,NKK,NUR
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),CRA(*),ICA(MAXICA),SUR(N),US(N),VS(N)
C     ..
C     .. Local Scalars ..
      REAL CC
      INTEGER COEF,I,IA,II,IUKTH,IUKTHN,J,K,KK,KKN,KTH0,KTHN
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT
C     ..
C     .. Data statements ..
      DATA COEF/3/
C     ..
      KK = NKK
      KTH0 = KTH
      IF (KTH0.EQ.0) KTH0 = 1
      DO 200 II = 1,NUR
          I = SUR(II)
          CC = 0.
   50     CC = CC + FLOAT(COEF*NFEAS)
          KTHN = CC*FLOAT(KTH0)
          IUKTHN = KTHN + US(I)
          IUKTH = KTH + US(I)
          K = I
          DO 100 J = 1,N
              IA = A(I,J) - VS(J)
              IF (IA.LE.IUKTHN) THEN
                  IF (IA.GT.IUKTH) THEN
                      KK = KK + 1
                      IF (KK.GT.MAXICA) GO TO 300
                      KKN = KK + N
                      CRA(K) = KKN
                      CRA(KKN) = 0
                      ICA(KK) = J
                  END IF
              END IF
  100     CONTINUE
          IF (CRA(I).EQ.0) GO TO 50
  200 CONTINUE
      NKK = KK
      KFEAS = 1
      RETURN
  300 KFEAS = -1
      RETURN
      END
C
      SUBROUTINE INDUS3(N,A,F,M,U,V,FB,FU,MAX,INF)
C
C SEARCH FOR AN INITIAL DUAL SOLUTION ( U(I),V(J) ) AND AN INITIAL
C PARTIAL PRIMAL SOLUTION ( F(I),FB(J) ) OF THE AP PROBLEM
C
C     .. Scalar Arguments ..
      INTEGER INF,M,MAX,N
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),F(N),FB(N),FU(N),U(N),V(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,IA,II,IMIN,J,J1,JJ,JMIN,L,MAXL,MIN
C     ..
      M = 0
      DO 100 J = 1,N
          U(J) = 0
          MIN = INF
          DO 50 I = 1,N
              IA = A(I,J)
              IF (IA.LE.MIN) THEN
                  IF (IA.GE.MIN) THEN
                      IF (F(I).NE.0) GO TO 50
                  END IF
                  MIN = IA
                  IMIN = I
              END IF
   50     CONTINUE
          V(J) = MIN
          IF (F(IMIN).EQ.0) THEN
              M = M + 1
              FB(J) = IMIN
              F(IMIN) = J
              FU(IMIN) = J + 1
          END IF
  100 CONTINUE
      MAX = 0
      DO 400 I = 1,N
          IF (F(I).NE.0) GO TO 400
          MIN = INF
          MAXL = 0
          DO 150 J = 1,N
              L = A(I,J) - V(J)
              IF (L.GT.MAX) MAXL = L
              IF (L.LE.MIN) THEN
                  IF (L.GE.MIN) THEN
                      IF (FB(J).NE.0) GO TO 150
                      IF (FB(JMIN).EQ.0) GO TO 150
                  END IF
                  MIN = L
                  JMIN = J
              END IF
  150     CONTINUE
          U(I) = MIN
          IF (MAXL-MIN.GT.MAX) MAX = MAXL - MIN
          J = JMIN
          IF (FB(J).EQ.0) GO TO 300
          DO 200 J = JMIN,N
              IF (A(I,J)-V(J).LE.MIN) THEN
                  II = FB(J)
                  J1 = FU(II)
                  IF (J1.LE.N) THEN
                      DO 155 JJ = J1,N
                          IF (FB(JJ).LE.0) THEN
                              IF (A(II,JJ)-U(II).EQ.V(JJ)) GO TO 250
                          END IF
  155                 CONTINUE
                      FU(II) = N + 1
                  END IF
              END IF
  200     CONTINUE
          GO TO 400
  250     F(II) = JJ
          FB(JJ) = II
          FU(II) = JJ + 1
  300     M = M + 1
          FB(J) = I
          F(I) = J
          FU(I) = J + 1
  400 CONTINUE
      RETURN
      END
C
      SUBROUTINE APMMIX(N,A,F,Z,FB,DUALU,DUALV,PRE,UV,R,DM,DP,INF)
C
C    SHORTEST AUGMENTING PATH AND HUNGARIAN METHOD FOR COMPLETE MATRICES
C
C     .. Scalar Arguments ..
      INTEGER INF,N,Z
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),DM(N),DP(N),DUALU(N),DUALV(N),F(N),FB(N),PRE(N),
     +        R(N),UV(N)
C     ..
C     .. Local Scalars ..
      INTEGER D,I,I1,I2,I2P1,II,IIX,IND,INDEX,INDEXV,IV,NUV,U,VGL,W
C     ..
      DO 400 U = 1,N
          IF (F(U).GT.0) GO TO 400
          DO 50 I = 1,N
              PRE(I) = U
              UV(I) = I
              DP(I) = INF
              DM(I) = A(U,I) - DUALU(U) - DUALV(I)
   50     CONTINUE
          NUV = N
          I2 = 0
          R(1) = U
          DP(U) = 0
  100     D = INF
          INDEX = 0
          I1 = I2
          DO 150 IV = 1,NUV
              I = UV(IV)
              IF (DM(I).LE.D) THEN
                  IF (DM(I).NE.D) THEN
                      INDEX = 0
                      I2 = I1
                  END IF
                  D = DM(I)
                  IF (FB(I).LE.0) THEN
                      IF (D.EQ.0) GO TO 250
                      INDEX = I
                  END IF
                  I2 = I2 + 1
                  R(I2) = IV
              END IF
  150     CONTINUE
          IF (INDEX.GT.0) GO TO 300
          I1 = I1 + 1
          I2P1 = I2 + I1
          DO 200 IIX = I1,I2
              II = I2P1 - IIX
              INDEXV = R(II)
              INDEX = UV(INDEXV)
              UV(INDEXV) = UV(NUV)
              NUV = NUV - 1
              W = FB(INDEX)
              DP(W) = D
              DO 160 IV = 1,NUV
                  I = UV(IV)
                  VGL = D + A(W,I) - DUALU(W) - DUALV(I)
                  IF (DM(I).GT.VGL) THEN
                      DM(I) = VGL
                      PRE(I) = W
                  END IF
  160         CONTINUE
  200     CONTINUE
          GO TO 100
C
  250     INDEX = I
  300     W = PRE(INDEX)
          FB(INDEX) = W
          IND = F(W)
          F(W) = INDEX
          IF (W.EQ.U) THEN
C
              DO 320 I = 1,N
                  IF (DP(I).NE.INF) DUALU(I) = DUALU(I) + D - DP(I)
                  IF (DM(I).LT.D) DUALV(I) = DUALV(I) + DM(I) - D
  320         CONTINUE
          ELSE
              INDEX = IND
              GO TO 300
          END IF
  400 CONTINUE
C
      Z = 0
      DO 500 I = 1,N
          Z = Z + DUALU(I) + DUALV(I)
  500 CONTINUE
      RETURN
      END
C
      SUBROUTINE AGMHP(N,R,CR,ORDSP,IC,C,F,V,UR,Z,P,Q,HP,DM,FB,ZS,IMP,
     +                 INF)
C
C AGMHEAP SEARCH FOR AN AUGMENTING PATH STARTING AT ROW R, USING
C JOHNSON ALGORITHM, IMPLEMENTED VIA A HEAP-QUEUE
C
C     .. Scalar Arguments ..
      INTEGER IMP,INF,N,ORDSP,R,UR,Z,ZS
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CR(N+1),DM(N),F(N),FB(N),HP(N),IC(ORDSP),P(N),
     +        Q(N),V(N)
C     ..
C     .. Local Scalars ..
      INTEGER DELTA,DJ,DP1,DU,FB1,FBJ,H,HP1,HP2,HP3,I,IFIN,IND,INIZ,J,K,
     +        K2,NHP,W
C     ..
      DELTA = ZS - Z + UR
      IMP = 0
      F(R) = 0
      DO 100 I = 1,N
          FB(I) = 0
  100 CONTINUE
      DO 200 I = 1,N
          Q(I) = 0
          P(I) = 0
          J = F(I)
          IF (J.GT.0) FB(J) = I
          DM(I) = INF
  200 CONTINUE
      NHP = 0
      P(R) = 0
      DU = 0
      W = R
      INIZ = CR(W)
      IFIN = CR(W+1) - 1
      IF (INIZ.LE.IFIN) GO TO 500
      GO TO 700
  300 W = FB(I)
C
      INIZ = CR(W)
      IFIN = CR(W+1) - 1
      IF (INIZ.GT.IFIN) GO TO 700
C
      DO 400 H = INIZ,IFIN
          J = IC(H)
          IF (J.EQ.I) THEN
              DU = DU - C(H)
              GO TO 500
          END IF
  400 CONTINUE
C
  500 DO 600 H = INIZ,IFIN
          J = IC(H)
          DJ = DU + C(H) - V(J)
          IF (W.NE.R) DJ = DJ + V(I)
          IF (DM(J).GT.DJ) THEN
              DM(J) = DJ
              P(J) = W
              FBJ = FB(J)
              IF (Q(J).EQ.0) THEN
C
                  NHP = NHP + 1
                  Q(J) = NHP
              END IF
C
              K = Q(J)
  520         K2 = K/2
              IF (K2.GT.0) THEN
                  HP2 = HP(K2)
                  IF (DJ.LE.DM(HP2)) THEN
                      IF (DJ.GE.DM(HP2)) THEN
                          IF (FBJ.NE.0 .OR. FB(HP2).EQ.0) GO TO 540
                      END IF
                      HP(K) = HP2
                      Q(HP2) = K
                      K = K2
                      GO TO 520
                  END IF
              END IF
  540         HP(K) = J
              Q(J) = K
          END IF
  600 CONTINUE
C
  700 I = HP(1)
      DU = DM(I)
      IF (DU.GT.DELTA) GO TO 1000
      IF (FB(I).EQ.0) THEN
  750     W = P(I)
          FB(I) = W
          IND = F(W)
          F(W) = I
          IF (W.NE.R) THEN
C
              I = IND
              GO TO 750
          ELSE
C
             DO 760 I = 1,N
                  IF (DM(I).LT.DU) V(I) = V(I) + DM(I) - DU
  760         CONTINUE
              Z = Z + DU - UR
              RETURN
          END IF
      ELSE
          Q(I) = 0
          NHP = NHP - 1
C
          IF (NHP.LT.0) THEN
C
              IMP = 1
              RETURN
          ELSE IF (NHP.EQ.0) THEN
              GO TO 300
          ELSE
C
              HP1 = HP(NHP+1)
              DP1 = DM(HP1)
              FB1 = FB(HP1)
              K = 1
          END IF
      END IF
  800 K2 = K*2
      IF (K2.LT.NHP) THEN
          HP2 = HP(K2)
          HP3 = HP(K2+1)
          IF (DM(HP2).GE.DM(HP3)) THEN
              IF (DM(HP2).NE.DM(HP3) .OR. FB(HP2).NE.0) THEN
                  HP2 = HP3
                  K2 = K2 + 1
              END IF
          END IF
      ELSE IF (K2.NE.NHP) THEN
          GO TO 900
      END IF
      HP2 = HP(K2)
      IF (DP1.LE.DM(HP2)) THEN
          IF (DP1.LT.DM(HP2)) GO TO 900
          IF (FB1.EQ.0 .OR. FB(HP2).NE.0) GO TO 900
      END IF
      HP(K) = HP2
      Q(HP2) = K
      K = K2
      GO TO 800
  900 HP(K) = HP1
      Q(HP1) = K
      GO TO 300
 1000 Z = INF
      RETURN
      END
C
      SUBROUTINE ENLINI(N,CR,IC,F,U,V,S,P,FLAG,CONT,A,CRA,ICA,NC,INF)
C
C     TRY TO CONNECT SUBTOURS OF THE SOLUTION OF THE ASSIGNMENT  AT
C     THE ROOT NODE, WITH ZERO REDUCED COST ARCS
C
C     .. Scalar Arguments ..
      INTEGER INF,N,NC
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),CONT(N),CR(N+1),CRA(*),F(N),FLAG(N),IC(*),ICA(*),
     +        P(N),S(N),U(N),V(N)
C     ..
C     .. Local Scalars ..
      INTEGER CARD,CORID,FLAGA,FSALVA,I,IALL,II,IJ,IJN,IK,IOLDS,IP1,IP2,
     +        ISTART,J,K,KC,KK,L,LM,LM2,M1,NC2,NINF2,PJ,SI,ULT
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT,IABS
C     ..
      NINF2 = -FLOAT(INF)/2.
      M1 = -1
      KC = 0
      IALL = 0
      DO 100 I = 1,N
          J = F(I)
          S(I) = J
          P(J) = I
          FLAG(I) = 0
          CONT(I) = 0
  100 CONTINUE
      DO 200 ISTART = 1,N
          IF (FLAG(ISTART).NE.0) GO TO 200
          KC = KC + 1
          K = ISTART
          CARD = 0
  150     FLAG(K) = -KC
          K = S(K)
          IF (V(K).GT.NINF2) CARD = CARD + 1
          IF (K.NE.ISTART) GO TO 150
          IF (CARD.NE.N) THEN
              J = 2*KC
              DO 160 I = 1,J,2
                  IF (CONT(I).LE.CARD) THEN
                      IF (CONT(I).NE.0) THEN
                          IP2 = I + 2
                          DO 152 L = IP2,J
                              LM = J + IP2 - L
                              LM2 = LM - 2
                              CONT(LM) = CONT(LM2)
  152                     CONTINUE
                      END IF
                      CONT(I) = CARD
                      IP1 = I + 1
                      CONT(IP1) = ISTART
                      GO TO 200
                  END IF
  160         CONTINUE
          ELSE
              NC = 1
              RETURN
          END IF
  200 CONTINUE
      NC = KC
      NC2 = NC*2
      DO 600 II = 2,NC2,2
          ISTART = CONT(II)
          IF (FLAG(ISTART).GT.0) GO TO 600
          FSALVA = IABS(FLAG(ISTART))
          FLAGA = 0
          I = ISTART
  250     J = F(I)
          IF (V(J).LE.NINF2) GO TO 550
          IF (CR(1).NE.M1) THEN
C
              IP1 = I + 1
              ULT = CR(IP1) - 1
              IK = I
              K = CR(I)
              J = IC(K)
              GO TO 400
          END IF
  300     K = 1
          ULT = N
  350     J = K
C
  400     IF (I.EQ.J) GO TO 500
  450     CORID = A(I,J) - U(I) - V(J)
          IF (CORID.LE.0) THEN
              IF (IABS(FLAG(J)).NE.FSALVA) THEN
                  IF (FLAGA.NE.0 .OR. FLAG(J).LE.0) THEN
                      PJ = P(J)
                      IF (V(J).GE.NINF2) THEN
                          SI = F(I)
                          IF (A(PJ,SI)-U(PJ).LE.V(SI)) THEN
C
                              FLAGA = 1
                              IALL = IALL + 1
                              NC = NC - 1
                              SI = F(I)
                              PJ = P(J)
                              F(I) = J
                              P(J) = I
                              F(PJ) = SI
                              P(SI) = PJ
                              IOLDS = S(I)
                              KK = J
                              IF (FLAG(J).GT.0) S(I) = PJ
                              IF (FLAG(J).LE.0) S(I) = J
                              S(PJ) = IOLDS
  452                         FLAG(KK) = FSALVA
                              KK = F(KK)
                              IF (KK.NE.SI) GO TO 452
                              IF (CR(1).EQ.M1) GO TO 300
                              K = CR(I)
                              J = IC(K)
                              GO TO 400
                          END IF
                      END IF
                  END IF
              END IF
          END IF
  500     K = K + 1
          IF (CR(1).NE.M1) THEN
              IF (K.LE.ULT) THEN
                  J = IC(K)
                  GO TO 400
              ELSE IF (CRA(IK).NE.0) THEN
                  IJ = CRA(IK)
                  IK = IJ
                  IJN = IJ - N
                  J = ICA(IJN)
                  GO TO 450
              END IF
          ELSE IF (K.LE.ULT) THEN
              GO TO 350
          END IF
  550     FLAG(I) = IABS(FLAG(I))
          I = S(I)
          IF (I.NE.ISTART) GO TO 250
  600 CONTINUE
      RETURN
      END
C
      SUBROUTINE ENLARG(N,CR,ORDSP,IC,C,F,U,VD,S,P,FLAG,CONT,NC,INF)
C
C     TRY TO CONNECT SUBTOURS OF THE CURRENT MODIFIED ASSIGNMENT
C     PROBLEM, WITH ZERO REDUCED COST ARCS
C
C
C     .. Scalar Arguments ..
      INTEGER INF,N,NC,ORDSP
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CONT(N),CR(N+1),F(N),FLAG(N),IC(ORDSP),P(N),S(N),
     +        U(N),VD(N)
C     ..
C     .. Local Scalars ..
      INTEGER CARD,CORID,FLAGA,FSALVA,I,IALL,II,INCR,IOLDS,IP1,IP2,IPJ,
     +        ISTART,J,JJ,K,KC,KK,L,LM,LM2,M1,META,MINF2,NC2,PJ,SI,ULT,
     +        ULT2
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT,IABS
C     ..
      MINF2 = -FLOAT(INF)/2.
      KC = 0
      M1 = -1
      IALL = 0
      DO 100 I = 1,N
          J = F(I)
          S(I) = J
          P(J) = I
          FLAG(I) = 0
          CONT(I) = 0
  100 CONTINUE
      DO 200 ISTART = 1,N
          IF (FLAG(ISTART).NE.0) GO TO 200
          KC = KC + 1
          K = ISTART
          CARD = 0
  150     FLAG(K) = -KC
          K = S(K)
          IF (VD(K).GT.MINF2) CARD = CARD + 1
          IF (K.NE.ISTART) GO TO 150
          IF (CARD.NE.N) THEN
              J = 2*KC
              DO 160 I = 1,J,2
                  IF (CONT(I).LE.CARD) THEN
                      IF (CONT(I).NE.0) THEN
                          IP2 = I + 2
                          DO 152 L = IP2,J
                              LM = J + IP2 - L
                              LM2 = LM - 2
                              CONT(LM) = CONT(LM2)
  152                     CONTINUE
                      END IF
                      CONT(I) = CARD
                      IP1 = I + 1
                      CONT(IP1) = ISTART
                      GO TO 200
                  END IF
  160         CONTINUE
          ELSE
              NC = 1
              RETURN
          END IF
  200 CONTINUE
      NC = KC
      NC2 = NC*2
      DO 400 II = 2,NC2,2
          ISTART = CONT(II)
          IF (FLAG(ISTART).GT.0) GO TO 400
          FSALVA = IABS(FLAG(ISTART))
          FLAGA = 0
          I = ISTART
  250     J = F(I)
          IF (VD(J).LE.MINF2) GO TO 350
          IP1 = I + 1
          ULT = CR(IP1) - 1
          K = CR(I)
  300     J = IC(K)
          CORID = C(K) - U(I) - VD(J)
          IF (CORID.LE.0) THEN
              IF (IABS(FLAG(J)).NE.FSALVA) THEN
                  IF (FLAGA.NE.0 .OR. FLAG(J).LE.0) THEN
                      PJ = P(J)
                      IF (VD(J).GE.MINF2) THEN
                          SI = F(I)
                          IPJ = PJ + 1
                          ULT2 = CR(IPJ) - 1
                          KK = CR(PJ)
                          META = FLOAT(IC(ULT2)-IC(KK))/2.
                          IF (SI.GT.META) THEN
                              IPJ = PJ + 1
                              KK = CR(IPJ) - 1
                              ULT2 = CR(PJ)
                              INCR = -1
                          ELSE
                              INCR = 1
                          END IF
  302                     JJ = IC(KK)
                          IF (JJ.EQ.SI) THEN
                              IF (C(KK)-U(PJ).LE.VD(SI)) THEN
                                  FLAGA = 1
                                  IALL = IALL + 1
                                  NC = NC - 1
                                  SI = F(I)
                                  PJ = P(J)
                                  F(I) = J
                                  P(J) = I
                                  F(PJ) = SI
                                  P(SI) = PJ
                                  IOLDS = S(I)
                                  KK = J
                                  IF (FLAG(J).GT.0) S(I) = PJ
                                  IF (FLAG(J).LE.0) S(I) = J
                                  S(PJ) = IOLDS
  304                             FLAG(KK) = FSALVA
                                  KK = F(KK)
                                  IF (KK.NE.SI) GO TO 304
                                  K = CR(I)
                                  GO TO 300
                              END IF
                          ELSE IF ((JJ.LE.SI) .OR. (INCR.NE.1)) THEN
                              IF ((JJ.GE.SI) .OR. (INCR.NE.M1)) THEN
                                  IF (KK.NE.ULT2) THEN
                                      KK = KK + INCR
                                      GO TO 302
                                  END IF
                              END IF
                          END IF
                      END IF
                  END IF
              END IF
          END IF
          K = K + 1
          IF (K.LE.ULT) GO TO 300
  350     FLAG(I) = IABS(FLAG(I))
          I = S(I)
          IF (I.NE.ISTART) GO TO 250
  400 CONTINUE
      RETURN
      END
C
      SUBROUTINE KARP(N,A,F,P,FLAG,NA,ND,TOTASS,UB,INF)
C
C     HEURISTIC SOLUTION OF THE ATSP USING THE PATCHING ALGORITHM
C
C
C     .. Scalar Arguments ..
      INTEGER INF,N,TOTASS,UB
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),F(N),FLAG(N),NA(N),ND(N),P(N)
C     ..
C     .. Local Scalars ..
      INTEGER CONT,COSTO,FSALVA,I,IM1,ISTART,J,K,KK,KP,MIN,MN1,MN2,MN3,
     +        NC,NIM1,PMN1,RR,S2,TEMP,Z
C     ..
      DO 100 I = 1,N
          FLAG(I) = 0
  100 CONTINUE
      CONT = 0
      NC = 0
      DO 200 I = 1,N
          IF (FLAG(I).LE.0) THEN
              K = I
              NC = NC + 1
  120         FLAG(K) = NC
              CONT = CONT + 1
              KK = F(K)
              P(KK) = K
              K = KK
              IF (K.NE.I) GO TO 120
              NA(NC) = CONT
              ND(NC) = I
              CONT = 0
          END IF
  200 CONTINUE
      IF (NC.EQ.1) RETURN
C
C SORT THE SUBTOURS
C
      UB = TOTASS
      Z = NC
  300 MIN = N
      DO 400 I = 1,Z
          IF (NA(I).LT.MIN) THEN
              MIN = NA(I)
              K = I
          END IF
  400 CONTINUE
      NA(K) = NA(Z)
      NA(Z) = MIN
      TEMP = ND(K)
      ND(K) = ND(Z)
      ND(Z) = TEMP
      Z = Z - 1
      IF (Z.GE.2) GO TO 300
      RR = 2
  500 ISTART = ND(RR)
      I = ISTART
      IM1 = RR - 1
      NIM1 = ND(IM1)
      FSALVA = FLAG(NIM1)
      MN2 = INF
  600 DO 700 K = 1,N
          IF (FLAG(K).EQ.FSALVA) THEN
              J = F(I)
              KP = P(K)
              COSTO = A(I,K) + A(KP,J) - A(KP,K) - A(I,J)
              IF (COSTO.LT.MN2) THEN
                  MN1 = K
                  MN2 = COSTO
                  MN3 = I
                  IF (COSTO.EQ.0) GO TO 800
              END IF
          END IF
  700 CONTINUE
      I = F(I)
      IF (I.NE.ISTART) GO TO 600
  800 S2 = F(MN3)
      PMN1 = P(MN1)
      F(MN3) = MN1
      P(MN1) = MN3
      F(PMN1) = S2
      P(S2) = PMN1
      UB = UB + MN2
C
C UPDATE FLAG
C
      FSALVA = FLAG(ISTART)
      KK = MN1
  900 FLAG(KK) = FSALVA
      KK = F(KK)
      IF (KK.NE.S2) GO TO 900
      NC = NC - 1
      IF (NC.EQ.1) RETURN
      RR = RR + 1
      GO TO 500
      END
C
      SUBROUTINE CERCSB(F,FLAG,VD,N,CMIN,ISALVA,INF)
C
C CHOSE THE SUBTOUR FOR THE BRANCH PHASE
C
C
C     .. Scalar Arguments ..
      INTEGER CMIN,INF,ISALVA,N
C     ..
C     .. Array Arguments ..
      INTEGER F(N),FLAG(N),VD(N)
C     ..
C     .. Local Scalars ..
      INTEGER CARD,I,ISTART,K,KC,MINF2
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT
C     ..
      MINF2 = -FLOAT(INF)/2.
      CMIN = INF
      KC = 0
      DO 100 I = 1,N
          FLAG(I) = 0
  100 CONTINUE
      DO 200 ISTART = 1,N
          IF (FLAG(ISTART).NE.0) GO TO 200
          KC = KC + 1
          CARD = 0
          K = ISTART
  150     FLAG(K) = KC
          K = F(K)
          IF (VD(K).GE.MINF2) CARD = CARD + 1
          IF (K.NE.ISTART) GO TO 150
          IF (CARD.LT.CMIN) THEN
              CMIN = CARD
              ISALVA = ISTART
          END IF
  200 CONTINUE
      RETURN
      END
C
      SUBROUTINE CLEARQ(ZSTAR,PUNTB,FMVF,ORDX,X,NCODAL,ORDV,V,ACTIVE)
C
C     MAKE NOT ACTIVE NODES WITH LB GREATER THAN ZSTAR
C
C
C     .. Scalar Arguments ..
      INTEGER ACTIVE,FMVF,NCODAL,ORDV,ORDX,PUNTB,ZSTAR
C     ..
C     .. Array Arguments ..
      INTEGER V(ORDV),X(ORDX)
C     ..
C     .. Local Scalars ..
      INTEGER J
C     ..
  100 IF (NCODAL.EQ.0) RETURN
      IF (V(PUNTB+1).LT.ZSTAR) RETURN
      J = V(PUNTB+4)
      X(J) = FMVF
      FMVF = J
      PUNTB = V(PUNTB)
      NCODAL = NCODAL - 1
      ACTIVE = ACTIVE - 1
      GO TO 100
      END
C
      SUBROUTINE INQUE(ISALVA,NNODI,SC1,PSALVO,NGEN,VD,F,N,TOTASS,MV,MF,
     +                 MM2,VIMPA,VIMPB,ORDV,V,NCODAL,PUNTLV,PUNTA,PUNTB,
     +                 IOFFV,INF)
C
C  PSALVO = POINTER TO THE FATHER
C  NNODI  = NUMBER OF NODES IN THE SUBTOUR
C
C
C
C     .. Scalar Arguments ..
      INTEGER INF,IOFFV,ISALVA,MM2,N,NCODAL,NGEN,NNODI,ORDV,PSALVO,
     +        PUNTA,PUNTB,PUNTLV,SC1,TOTASS,VIMPA,VIMPB
C     ..
C     .. Array Arguments ..
      INTEGER F(N),MF(N),MV(N),V(ORDV),VD(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,I32,KSALVA,LB,LB1,MINF2,NM1,
     +        NODOB,NODOPA,PCORR,PLVP,PUNTOL,SC2,SC3
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT
C     ..
      I32 = 32000
C
C     INSERT THE PROBLEM IN THE QUEUE IN THIS ORDER
C     (FROM THE THIRD LOCATION):
C     LB,
C     PSALVO,
C     THE GENERATION NUMBER (AMONG THE SONS OF THE SAME FATHER) AND THE
C       NUMBER OF NOT IMPOSED ARCS IN THE SUBTOUR CHOOSEN FOR THE BRANCH
C       PHASE (PACKED),
C     THE POINTER TO THE COLUMN OF MATRICES MV,MF,
C     THE ADDITIONAL ARC TO EXCLUDE FROM SOLUTION
C       (START NODE + END NODE, PACKED),
C     THE SCORE SC1 ((N.OF EXCLUDED ARCS)*10+NUMBER OF IMPOSED ARCS),
C     THE NOT IMPOSED ARCS IN THE SUBTOUR CHOOSEN FOR THE
C       BRANCH PHASE (PACKED)
C
      MINF2 = -FLOAT(INF)/2.
      NM1 = NGEN - 1
      SC1 = SC1 + (NM1)*10 + 1
      V(PUNTLV+2) = TOTASS
      V(PUNTLV+3) = PSALVO
      V(PUNTLV+4) = NGEN*I32 + NNODI
      V(PUNTLV+5) = MM2
      DO 100 I = 1,N
          MV(I) = VD(I)
          MF(I) = F(I)
  100 CONTINUE
      V(PUNTLV+6) = VIMPA*I32 + VIMPB
      V(PUNTLV+7) = SC1
      PLVP = PUNTLV + IOFFV
      I = 1
      KSALVA = ISALVA
      NODOPA = ISALVA
  200 NODOB = F(NODOPA)
      IF (VD(NODOB).GE.MINF2) THEN
          V(PLVP+I) = NODOPA*I32 + NODOB
          I = I + 1
      END IF
      NODOPA = NODOB
      IF (NODOPA.NE.KSALVA) GO TO 200
C
C     PUNTLV: POINTER AT THE FIRST  EMPTY POSITION OF V()
C     PUNTA : POINTER AT THE ACTIVE PROBLEM WITH BEST LB
C     PUNTB : POINTER AT THE ACTIVE PROBLEM WITH WORST LB
C
      NCODAL = NCODAL + 1
      IF (NCODAL.EQ.1) THEN
C
C     INSERT THE FIRST PROBLEM
C
          PUNTA = PUNTLV
          PUNTB = PUNTLV + 1
      ELSE
          SC2 = V(PUNTA+IOFFV)
          SC3 = V(PUNTB+IOFFV-1)
          LB = TOTASS
          IF (LB.LT.V(PUNTA+2) .OR. (LB.EQ.V(PUNTA+2).AND.(SC1.GE.SC2)))
     +        THEN
C
              V(PUNTLV) = PUNTA
              V(PUNTA+1) = PUNTLV + 1
              PUNTA = PUNTLV
          ELSE IF (LB.GT.V(PUNTB+1) .OR. (LB.EQ.V(PUNTB+1).AND.
     +            (SC1.LE.SC3)))  THEN
C
              V(PUNTLV+1) = PUNTB
              V(PUNTB-1) = PUNTLV
              PUNTB = PUNTLV + 1
          ELSE
C
C     FIND  THE FIRST ACTIVE PROBLEM WITH COST GREATER THAN LB
C
              PCORR = V(PUNTA)
  220         SC2 = V(PCORR+IOFFV)
              LB1 = V(PCORR+2)
              IF ((LB.LT.LB1) .OR. ((LB.EQ.LB1).AND. (SC1.GE.SC2))) THEN
C
C     INSERT THE PROBLEM IN QUEUE
C
                  PUNTOL = V(PCORR+1) - 1
                  V(PUNTOL) = PUNTLV
                  V(PUNTLV) = PCORR
                  V(PCORR+1) = PUNTLV + 1
                  V(PUNTLV+1) = PUNTOL + 1
              ELSE
                  PCORR = V(PCORR)
                  GO TO 220
              END IF
          END IF
      END IF
      PUNTLV = PUNTLV + IOFFV + NNODI + 1
      RETURN
      END
C
      SUBROUTINE EXQUE(ORDV,V,ORDX,X,FMVF,VD,F,P2,NGEN,PUNTA,NPROB,
     +                 IVA,IVB,NCODAL,N,SC1,TOTASS,IOFFV)
C
C     .. Scalar Arguments ..
      INTEGER FMVF,IOFFV,N,NCODAL,NGEN,NPROB,ORDV,ORDX,P2,PUNTA,SC1,
     +        TOTASS
C     ..
C     .. Array Arguments ..
      INTEGER F(N),IVA(N),IVB(N),V(ORDV),VD(N),X(ORDX)
C     ..
C     .. Local Scalars ..
      REAL R32
      INTEGER I,I32,IX,J,JP
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT
C     ..
      I32 = 32000
      R32 = 32000.
      P2 = PUNTA
      NGEN = FLOAT(V(PUNTA+4))/R32
      NPROB = V(PUNTA+4) - NGEN*I32
      TOTASS = V(PUNTA+2)
      JP = V(PUNTA+5)
      J = 1
      DO 100 I = 1,NPROB
          IX = FLOAT(V(PUNTA+IOFFV+I))/R32
          IVA(J) = IX
          IVB(J) = V(PUNTA+IOFFV+I) - IX*I32
          J = J + 1
  100 CONTINUE
C
C   LOAD THE NEW VECTORS VD AND F
C
      DO 200 I = 0,N - 1
          VD(I+1) = X(JP+I)
          F(I+1) = X(JP+N+I)
  200 CONTINUE
C
      X(JP) = FMVF
      FMVF = JP
      SC1 = V(PUNTA+IOFFV)
      PUNTA = V(PUNTA)
      NCODAL = NCODAL - 1
      RETURN
      END
C
      SUBROUTINE ACTPRO(PATT,N,NPROBV,PNUOVO,NGEN,VIMPA,VIMPB,ORDV,V,CR,
     +                  ORDSP,IC,C,IOFFV,INF)
C
C     .. Scalar Arguments ..
      INTEGER INF,IOFFV,N,NGEN,NPROBV,ORDSP,ORDV,PATT,PNUOVO,VIMPA,VIMPB
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CR(N+1),IC(ORDSP),V(ORDV)
C     ..
C     .. Local Scalars ..
      REAL R32
      INTEGER I32,M1,NGENOL,NGENPP,NODOA,NODOB,NODOC,NODOD,P1,POLD,PP
C     ..
C     .. External Subroutines ..
      EXTERNAL MODMAT
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC FLOAT
C     ..
      M1 = -1
      P1 = 1
      I32 = 32000
      R32 = 32000.
C
C     UPDATE THE COST MATRIX FROM THE ACTUAL ONE(ASSOCIATED WITH PATT)
C     TO THE NEW ONE(ASSOCIATED WITH PNUOVO)
C
      IF (V(PNUOVO+3).NE.PATT) THEN
C
C     THE PROBLEM PATT IS NOT THE FATHER OF PNUOVO
C
C     "MARK" THE STREET FROM PNUOVO TO THE ROOT NODE
C
          PP = PNUOVO
   50     IF (PP.EQ.0) THEN
              NGENOL = NPROBV
C
C      FIND A MARKED PROBLEM
C
   60         NODOA = FLOAT(V(PATT + IOFFV + NGENOL))/R32
              NODOB = V(PATT + IOFFV + NGENOL) - NODOA*I32
              NODOC = VIMPA
              NODOD = VIMPB
C
C     UPDATE THE MATRIX FROM THE ONE ASSOCIATED WITH THE SON OF PATT
C     TO THE ONE ASSOCIATED WITH PATT
C
              CALL MODMAT(CR,ORDSP,IC,C,NODOA,NODOB,M1,INF)
              IF (NODOC.NE.0) CALL MODMAT(CR,ORDSP,IC,C,NODOC,NODOD,M1,
     +                                    INF)
              IF (V(PATT+5).LE.0) THEN
C
C     THE ACTUAL MATRIX (ASSOCIATED WITH THE PROBLEM PATT)
C     IS THE ONE ASSOCIATED WITH COMMON PREDECESSOR OF PNEW
C     AND THE FIRST PATT
C
C     POLD : COMMON PREDECESSOR PROBLEM
C
                  PP = PNUOVO
                  POLD = PATT
C
   70             PATT = V(PP+3)
                  NGENPP = FLOAT(V(PP+4))/R32
C
C     PROHIBIT ARCS
C
                  NODOA = FLOAT(V(PATT + IOFFV + NGENPP))/R32
                  NODOB = V(PATT + IOFFV + NGENPP) - NODOA*I32
                  NODOC = FLOAT(V(PP+6))/R32
                  NODOD = V(PP+6) - NODOC*I32
                  CALL MODMAT(CR,ORDSP,IC,C,NODOA,NODOB,P1,INF)
                  IF (NODOC.NE.0) CALL MODMAT(CR,ORDSP,IC,C,NODOC,NODOD,
     +                                 P1,INF)
                  IF (PATT.EQ.POLD) THEN
C
C     UPDATE THE MARKED PROBLEMS
C
                      PP = PNUOVO
   75                 V(PP+5) = -V(PP+5)
                      PP = V(PP+3)
                      IF (PP.NE.0) GO TO 75
                  ELSE
                      PP = PATT
                      GO TO 70
                  END IF
              ELSE
                  NGENOL = FLOAT(V(PATT+4))/R32
                  VIMPA = FLOAT(V(PATT+6))/R32
                  VIMPB = V(PATT+6) - VIMPA*I32
                  PATT = V(PATT+3)
                  GO TO 60
              END IF
          ELSE
C
              V(PP+5) = -V(PP+5)
              PP = V(PP+3)
              GO TO 50
          END IF
      ELSE
C
C     THE PROBLEM PATT IS THE FATHER OF PNUOVO
C
          IF (NGEN.EQ.NPROBV) RETURN
          NODOA = FLOAT(V(PATT + IOFFV + NPROBV))/R32
          NODOB = V(PATT + IOFFV + NPROBV) - NODOA*I32
C
C      UPDATE THE COST OF THE PROHIBITED ARC
C
          CALL MODMAT(CR,ORDSP,IC,C,NODOA,NODOB,M1,INF)
          NODOC = VIMPA
          NODOD = VIMPB
C
C     UPDATE THE COST OF THE ''IMPLICIT'' PROHIBITED ARC
C
          IF (NODOC.NE.0) CALL MODMAT(CR,ORDSP,IC,C,NODOC,NODOD,M1,
     +                                INF)
          NODOA = FLOAT(V(PATT + IOFFV + NGEN))/R32
          NODOB = V(PATT + IOFFV + NGEN) - NODOA*I32
C
C     PROHIBIT THE ARC NODOA,NODOB
C
          CALL MODMAT(CR,ORDSP,IC,C,NODOA,NODOB,P1,INF)
          NODOC = FLOAT(V(PNUOVO+6))/R32
          NODOD = V(PNUOVO+6) - NODOC*I32
C
C     PROHIBIT THE ''IMPLICIT''ARC
C
          IF (NODOC.NE.0) CALL MODMAT(CR,ORDSP,IC,C,NODOC,NODOD,P1,
     +                                INF)
          RETURN
      END IF
      RETURN
      END
C
      SUBROUTINE GENSON(N,IVA,IVB,VD,NPROB,I,VIMPA,VIMPB,NODOIM,CR,
     +                  ORDSP,IC,C,INF)
C
C     .. Scalar Arguments ..
      INTEGER I,INF,N,NODOIM,NPROB,ORDSP,VIMPA,VIMPB
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CR(N+1),IC(ORDSP),IVA(N),IVB(N),VD(N)
C     ..
C     .. Local Scalars ..
      INTEGER I2,IBM1,IM1,M1,P1
C     ..
C     .. External Subroutines ..
      EXTERNAL MODMAT
C     ..
      M1 = -1
      P1 = 1
      VIMPA = 0
      VIMPB = 0
C
C      PROHIBIT I-TH ARC
C
      CALL MODMAT(CR,ORDSP,IC,C,IVA(I),IVB(I),P1,INF)
      IF (I.NE.1) THEN
          IM1 = I - 1
          IBM1 = IVB(IM1)
          I2 = IBM1
          CALL MODMAT(CR,ORDSP,IC,C,IVA(IM1),I2,M1,INF)
          VD(IBM1) = VD(IBM1) - INF
C
C    DON'T PROHIBIT 2 TIMES THE SAME ARC
C
          IF (NODOIM.NE.IVB(I)) THEN
              I2 = NODOIM
              CALL MODMAT(CR,ORDSP,IC,C,IVA(I),I2,P1,INF)
              VIMPA = IVA(I)
              VIMPB = NODOIM
          ELSE
              VIMPA = 0
              VIMPB = 0
          END IF
      ELSE
          NODOIM = IVB(NPROB)
          RETURN
      END IF
C
C    IF NECESSARY ADJUST THE 'IMPLICIT' PROHIBITED ARC
C
      I2 = NODOIM
      IF (I.GE.3) CALL MODMAT(CR,ORDSP,IC,C,IVA(IM1),I2,M1,INF)
      RETURN
      END
C
      SUBROUTINE CALCUR(N,CR,ORDSP,IC,C,F,IVAI,VD,UR,INF)
C
C   COMPUTE THE VALUE OF THE DUAL VARIABLE ASSOCIATED WITH ROW IVAI
C
C     .. Scalar Arguments ..
      INTEGER INF,IVAI,N,ORDSP,UR
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CR(N+1),F(N),IC(ORDSP),VD(N)
C     ..
C     .. Local Scalars ..
      INTEGER JK,KL,PR,UL,VR
C     ..
      JK = F(IVAI)
      VR = VD(JK)
      PR = CR(IVAI)
      UL = CR(IVAI+1) - 1
      DO 100 KL = PR,UL
          IF (IC(KL).EQ.JK) THEN
              UR = C(KL) - VR - INF
              GO TO 200
          END IF
  100 CONTINUE
  200 RETURN
      END
C
      SUBROUTINE CALCUD(N,ORDSP,IC,CR,C,VC,FC,UC)
C
C  COMPUTE THE CURRENT DUAL VARIABLES U
C
C     .. Scalar Arguments ..
      INTEGER N,ORDSP
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CR(N+1),FC(N),IC(ORDSP),UC(N),VC(N)
C     ..
C     .. Local Scalars ..
      INTEGER I,IFS,II,ILS,JK
C     ..
      DO 100 I = 1,N
          JK = FC(I)
          IFS = CR(I)
          ILS = CR(I+1) - 1
          DO 50 II = IFS,ILS
              IF (IC(II).EQ.JK) UC(I) = C(II) - VC(JK)
   50     CONTINUE
  100 CONTINUE
      RETURN
      END
C
      SUBROUTINE MODMAT(CR,ORDSP,IC,C,NODOA,NODOB,FLAG,INF)
C
C     IF FLAG=1  EXCLUDE ARC (NODOA,NODOB)
C     IF FLAG=-1 REMOVE EXCLUSION CONSTRAINT FORM ARC (NODOA,NODOB)
C
C     .. Scalar Arguments ..
      INTEGER FLAG,INF,NODOA,NODOB,ORDSP
C     ..
C     .. Array Arguments ..
      INTEGER C(ORDSP),CR(*),IC(ORDSP)
C     ..
C     .. Local Scalars ..
      INTEGER J,K,PR,UL
C     ..
      PR = CR(NODOA)
      UL = CR(NODOA+1) - 1
      DO 100 J = PR,UL
          K = IC(J)
          IF (K.EQ.NODOB) THEN
              C(J) = C(J) + FLAG*INF
              GO TO 200
          END IF
  100 CONTINUE
  200 RETURN
      END
      SUBROUTINE CREAMS(GAP,A,N,CR,X,MM6,MM4,MM3,U,V,ZERI,NLSP,ERR)
C
C     REDUCTION PROCEDURE
C
C     STORE THE SPARSE MATRIX IN X() AND  DEFINES MM3,MM4
C     VECTOR IC() IS STORED FROM MM3 AND VECTOR C() FROM MM4
C
C     .. Scalar Arguments ..
      INTEGER ERR,GAP,MM3,MM4,MM6,N,NLSP,ZERI
C     ..
C     .. Array Arguments ..
      INTEGER A(N,N),CR(N+1),U(N),V(N),X(*)
C     ..
C     .. Local Scalars ..
      INTEGER GAP2,I,IA,IU,J,K,MIDDLE
C     ..
      ZERI = 0
      ERR = 0
C
      MM4 = MM6
      MIDDLE = (MM4+MM3)/2.
      K = MIDDLE
      DO 100 I = N,1,-1
          CR(I+1) = MM4
          IU = U(I)
          GAP2 = GAP + IU
          DO 50 J = N,1,-1
              IF (I.NE.J) THEN
                  IA = A(I,J) - V(J)
                  A(I,J) = IA - IU
                  IF (IA.LE.GAP2) THEN
                      IF (IA.EQ.IU) ZERI = ZERI + 1
                      MM4 = MM4 - 1
                      IF (MM4.LE.MIDDLE) GO TO 500
                      X(K) = J
                      X(MM4) = IA - U(I)
                      K = K - 1
                  END IF
              END IF
   50     CONTINUE
  100 CONTINUE
      CR(1) = MM4
      DO 200 I = 1,N + 1
          CR(I) = CR(I) - MM4 + 1
  200 CONTINUE
      NLSP = CR(N+1) - CR(1)
      K = MIDDLE
      MM3 = MM4 - 1
      DO 300 I = NLSP,1,-1
          X(MM3) = X(K)
          K = K - 1
          MM3 = MM3 - 1
  300 CONTINUE
      MM3 = MM3 + 1
      DO 400 I = 1,N
          U(I) = 0
          V(I) = 0
  400 CONTINUE
      RETURN
  500 ERR = 1
      RETURN
      END
      SUBROUTINE ERRORS(OUTERR,ERR)
C
C WRITE ON UNIT 6 A MESSAGE CORRESPONDING TO AN ERROR
C
C     .. Scalar Arguments ..
      INTEGER ERR,OUTERR
C     ..
      OUTERR = 1
      IF (ERR.NE.-1) THEN
          IF (ERR.EQ.1) WRITE (6,FMT=99002)
          IF (ERR.EQ.2) WRITE (6,FMT=99003)
          IF (ERR.EQ.3) WRITE (6,FMT=99004)
          IF (ERR.EQ.4) WRITE (6,FMT=99005)
          IF (ERR.EQ.5) WRITE (6,FMT=99006)
          IF (ERR.EQ.6) THEN
              WRITE (6,FMT=99007)
              RETURN
          END IF
      ELSE
          WRITE (6,FMT=99001)
          RETURN
      END IF
      WRITE (6,FMT='(//'' INCREASE PARAMETER ORDX'')')
      RETURN
99001 FORMAT (' MAXND NODES EXPLORED',/,/,
     +       ' *** SOLUTION NOT OPTIMAL ***')
99002 FORMAT (' INSUFFICENT MEMORY TO STORE THE WORKING VECTORS')
99003 FORMAT (
     +    ' INSUFFICENT MEMORY TO STORE THE INITIAL ASSIGNMENT PROBLEM')
99004 FORMAT (' INSUFFICIENT MEMORY TO STORE THE SPARSE MATRIX')
99005 FORMAT (' INSUFFICIENT MEMORY TO STORE THE ROOT NODE PROBLEM')
99006 FORMAT (' INSUFFICIENT MEMORY TO STORE THE BRANCH-DECISION-TREE')
99007 FORMAT (' THE ARTIFICIAL UPPER BOUND IS TOO SMALL',/,
     +       ' *** SOLUTION NOT OPTIMAL ***',/,
     +       ' INCREASE PARAMETER ALPHA')
      END
