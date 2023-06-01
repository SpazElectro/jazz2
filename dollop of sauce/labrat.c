
void labrat(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump,dx,px,py,hit1,hit2;
//	int calc;
//	int animspeed;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
		PutWalkerOnGround(num);
		//obj->xacc=0;
		obj->state=sWALK;
		obj->var4=30+RandFac(31);
		break;

	case sWALK:
		obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
		cWALK(num);

		obj->counter++; 

		if (obj->counter>AISPEED*8)
		{
			obj->counter=0;
			obj->phase=0;
			obj->state=sIDLE;
			obj->var4=RandFac(3);
			return;
		};

		if (RandFac(31)==0)
		{
			nump=RandFac(3);
			switch ((RandFac(3))&3)
			{
			case 0:
				PlaySample(obj->xpos,obj->ypos,sLABRAT_MOUSE1,0,0);
				break;
			case 1:
				PlaySample(obj->xpos,obj->ypos,sLABRAT_MOUSE2,0,0);
				break;
			case 2:
				PlaySample(obj->xpos,obj->ypos,sLABRAT_MOUSE3,0,0);
				break;

			default:
				break;
			}
		}; //sample

		if (obj->counter>AISPEED*2)
		{
			nump=GetNearestPlayer(num,128*128);
			if (nump>=0)
			{
				dx=player[nump].xpos-obj->xpos;
				if (((obj->direction<0) && (dx<0)) ||
					((obj->direction>0) && (dx>0)))
				{
					obj->state=sACTION;
					obj->xspeed=obj->direction*3*65536; //fast!
					obj->counter=0;
					obj->phase=0;
					obj->curanim=AnimBase[mLABRAT]+mLABRAT_ATTACK;
					obj->ypos-=4*65536;
					obj->yspeed=-3*65536;
				}
			}
		}
		break;


	case sACTION:	//attack!
    
		obj->xpos+=obj->xspeed;

		//CHECKING FOR BOUNCING OF WALLS OR RAVINES

		if (obj->direction<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		//py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		//hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(20<<16));
		hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos-8*65536);

		if (hit2==1)	//bounce of wall
		{
			obj->xpos-=obj->xspeed;
/*
			obj->xspeed=-obj->xspeed/4;
			obj->direction=-obj->direction;
*/
			obj->xspeed=0;
		}

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		if (obj->yspeed>0)
			py+=4; //land correctly
		//t1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py*65536));
		hit1=CheckPixel(obj->xpos+15*65536,obj->ypos+py*65536);

		if (hit1==0)
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/4;
			else
				obj->yspeed+=level.gravity;
			if (obj->yspeed>6*65536)
				obj->yspeed=6*65536;
			obj->ypos+=obj->yspeed;
		}
		else
		{
/*
			if (GetEvent(obj->xpos,obj->ypos-8*65536)==areaSTOPENEMY)
			{
				obj->counter=-AISPEED*2;
				obj->state=sEXTRA;
				//save speeds for reverse jump!
	
			} else
*/			{
				obj->yacc=0;
				obj->yspeed=0;
			}
		}

		obj->counter++;
		if (obj->counter>6)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=5)
			{
				if (hit1)
					obj->state=sLAND;
				else
					obj->phase=5;	//fall anim
			} else
			if (obj->phase==3)
				PlaySample(obj->xpos,obj->ypos,sLABRAT_BITE,0,0);
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sLAND:
		PutWalkerOnGround(num);

		obj->yspeed=0;
		obj->xspeed=0;
		obj->counter++;
		if (obj->counter>6)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->state=sWALK;
				obj->xspeed=0; //cWALK resets this
			}
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sEXTRA: //wrong jump ;-)
		obj->counter++;
		if (obj->counter<0)
		{
			obj->curanim=AnimBase[mLABRAT]+mLABRAT_IDLE;

			obj->phase=((AISPEED*2+obj->counter)/6)%anims[obj->curanim].numframes;
		}
		else
		if (obj->counter==0)
		{
			obj->curanim=AnimBase[mLABRAT]+mLABRAT_ATTACK;

			obj->direction=-obj->direction;
			obj->xspeed=-obj->xspeed;
			obj->yspeed=-obj->yspeed;
		}
		else
		{
			obj->xpos+=obj->xspeed;

			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/2;
			else
				obj->yspeed+=level.gravity*2;
			obj->ypos+=obj->yspeed;
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sIDLE:
		obj->curanim=AnimBase[mLABRAT]+mLABRAT_IDLE;

		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			if ((obj->phase==5) && (InViewArea(obj->xpos,obj->ypos))) 
			{
				if (RandFac(1)==1)
					PlaySample(obj->xpos,obj->ypos,sLABRAT_EYE3,0,0);
				else
					PlaySample(obj->xpos,obj->ypos,sLABRAT_EYE2,0,0);				
				
			}
			obj->counter=0;
		}

		if (obj->phase>=(int)anims[obj->curanim].numframes)
		{
			obj->var4--;
			if (obj->var4>0)
			{
				obj->phase=0;
			} else
			{
				obj->state=sWALK;
				obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
				obj->phase=0;
				obj->xspeed=0; //cWALK resets this
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
				if (obj->state==sWALK)
					obj->xspeed=0; //cWALK resets this
			}
		} else
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		DebugXMessage("labrat(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}	//switch

} //labrat()