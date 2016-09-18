#if !defined(__DLNET_H__259ADFCB_316B_4832_B14E_2928FCCBC10B__)
#define __DLNET_H__259ADFCB_316B_4832_B14E_2928FCCBC10B__

#pragma once
 
#include "resource.h"
#include <math.h>

#include <vector>
#include <list>
#include <map>

#include <Images/MemoryDC.h>
#include "utils.h"
#include <utils/tracealloc.h>
#include <utils/binfile.h>

#if !defined(real)
#define real double
#endif

struct Vol
{
	size_t sx,sy,depth;
	typedef std::vector<real> WT;
	WT w,dw;

	Vol(size_t _sx = 0,size_t _sy = 0,size_t _depth = 0,bool _bRandInit = true,real _c = 0)
		:sx(0),sy(0),depth(0)
	{
		alloc(_sx,_sy,_depth,_bRandInit,_c);
	}

	~Vol()
	{
		if(!w.empty()) w.clear();
		if(!dw.empty()) dw.clear();
	}

	void alloc(size_t _sx,size_t _sy,size_t _depth,bool _bRandInit,real _c = 0,const Vol* _vol = NULL)
	{
		sx = _sx;sy = _sy; depth = _depth;
		size_t n = sx*sy*depth;
		w.resize(n);
		dw.resize(n);
		if(_bRandInit)
		{
			real scale = sqrt((real)1.0/((real)sx*(real)sy*(real)depth));
			size_t i=0;
			gauss_rand randn;
			real measure = min_value<real>(scale*(real)3.0,(real)0.2);
			for(i=0;i<n;i++) {w[i] = clamp<real>(randn((real)0.0,scale),-measure,measure);}
		}
		else if(_vol==NULL)
		{
			size_t i=0;
			for(i=0;i<n;i++) {w[i] = _c;}
		}
		else if(_vol!=NULL)
		{
			size_t i=0;
			for(i=0;i<n;i++) {w[i] = _vol->w[i];}
			for(i=0;i<n;i++) {dw[i] = _vol->dw[i];}
		}
	}

	void clear()
	{
		if(!w.empty()) w.clear();
		if(!dw.empty()) dw.clear();
		sx = 0;
		sy = 0;
		depth = 0;
	}

	void alloc_copy(const Vol& _vol)
	{
		alloc(_vol.sx,_vol.sy,_vol.depth,false,(real)0,&_vol);
	}

	void copy_w(const Vol& _vol)
	{
		VERIFY_EXIT(w.size()==_vol.w.size());
		size_t i=0;
		for(i=0;i<w.size();i++) {w[i] = _vol.w[i];}
	}

	void copy_w_mask(const Vol& _vol,size_t _min_idx,size_t _max_idx)
	{
		VERIFY_EXIT(w.size()==_vol.w.size());
		size_t i=0;
		for(i=_min_idx;i<w.size() && i<_max_idx;i++) {w[i] = _vol.w[i];}
	}

	void copy_w_clamp(const Vol& _vol)
	{
		VERIFY_EXIT(w.size()==_vol.w.size());
		size_t i=0;
		for(i=0;i<w.size();i++) {w[i] = clamp(_vol.w[i],(real)-0.5,(real)0.5);}
	}

	void copy_dw(const Vol& _vol)
	{
		VERIFY_EXIT(dw.size()==_vol.dw.size());
		size_t i=0;
		for(i=0;i<dw.size();i++) {dw[i] = _vol.dw[i];}
	}

	void copy_dw_mask(const Vol& _vol,size_t _min_idx,size_t _max_idx)
	{
		VERIFY_EXIT(dw.size()==_vol.dw.size());
		size_t i=0;
		for(i=0;i<dw.size();i++) 
		{
			if(i>=_min_idx && i<_max_idx)
				dw[i] = _vol.dw[i];
			else
				dw[i] = (real)0.0;
		}
	}

	void copy_w(const Vol& _vol,size_t _offs1,size_t _offs2,size_t _sz)
	{
		VERIFY_EXIT(w.size()>=_offs1+_sz);
		VERIFY_EXIT(_vol.w.size()>=_offs2+_sz);
		size_t i=0;
		for(i=0;i<_sz;i++) {w[i+_offs1] = _vol.w[i+_offs2];}
	}

	void copy_dw(const Vol& _vol,size_t _offs1,size_t _offs2,size_t _sz)
	{
		VERIFY_EXIT(dw.size()>=_offs1+_sz);
		VERIFY_EXIT(_vol.dw.size()>=_offs2+_sz);
		size_t i=0;
		for(i=0;i<_sz;i++) {dw[i+_offs1] = _vol.dw[i+_offs2];}
	}

	void add_dw(const Vol& _vol)
	{
		VERIFY_EXIT(dw.size()==_vol.dw.size());
		size_t i=0;
		for(i=0;i<dw.size();i++) {dw[i] += _vol.dw[i];}
	}

	void add_w(const Vol& _vol)
	{
		VERIFY_EXIT(w.size()==_vol.w.size());
		size_t i=0;
		for(i=0;i<w.size();i++) {w[i] += _vol.w[i];}
	}

	void mull_w(real _k)
	{
		size_t i=0;
		for(i=0;i<w.size();i++) {w[i] *= _k;}
	}

	void mull_dw(real _k)
	{
		size_t i=0;
		for(i=0;i<dw.size();i++) {dw[i] *= _k;}
	}

	void norm_w()
	{
		size_t i=0;
		for(i=0;i<w.size();i++)
		{
			real maxv = w[i];
			real minv = w[i];
			maxv = max(maxv,(real)0.5);
			minv = fabs(min(minv,(real)-0.5));
			real delta = 2*max(maxv,minv);
			w[i] = w[i]/delta;
		}
		//for(i=1;i<w.size();i++)
		//{
		//	if(maxv<w[i]) maxv = w[i];
		//	if(minv>w[i]) minv = w[i];
		//}
		//minv = min(minv,-0.5);
		//maxv = max(maxv,0.5);
		//if(minv==0.5 && maxv==0.5) return;
		//real delta = 2*max(fabs(minv),fabs(maxv));
		//for(i=0;i<w.size();i++)
		//{
		//	w[i] = w[i]/delta;
		//}
	}

	void add_dw(const Vol& _vol,size_t _offs1,size_t _offs2,size_t _sz)
	{
		VERIFY_EXIT(dw.size()>=_offs1+_sz);
		VERIFY_EXIT(_vol.dw.size()>=_offs2+_sz);
		size_t i=0;
		for(i=0;i<dw.size();i++) {dw[i+_offs1] += _vol.dw[i+_offs2];}
	}

	void copy(const Vol& _vol)
	{
		VERIFY_EXIT(w.size()==_vol.w.size());
		size_t i=0;
		for(i=0;i<w.size();i++) {w[i] = _vol.w[i];}
		for(i=0;i<w.size();i++) {dw[i] = _vol.dw[i];}
	}

	real get(size_t _x,size_t _y,size_t _depth) const
	{
		size_t idx = ((sx*_y)+_x)*depth + _depth;
		return w[idx];
	}

	void set(size_t _x,size_t _y,size_t _depth,real _v)
	{
		size_t idx = ((sx*_y)+_x)*depth + _depth;
		w[idx] = _v;
	}

	void add(size_t _x,size_t _y,size_t _depth,real _v)
	{
		size_t idx = ((sx*_y)+_x)*depth + _depth;
		w[idx] += _v;
	}

	real get_grad(size_t _x,size_t _y,size_t _depth) const
	{
		size_t idx = ((sx*_y)+_x)*depth + _depth;
		return dw[idx];
	}

	void set_grad(size_t _x,size_t _y,size_t _depth,real _v)
	{
		size_t idx = ((sx*_y)+_x)*depth + _depth;
		dw[idx] = _v;
	};

	void add_grad(size_t _x,size_t _y,size_t _depth,real _v)
	{
		size_t idx = ((sx*_y)+_x)*depth + _depth;
		dw[idx] += _v;
	}

	void zero_dw()
	{
		size_t i=0;
		for(i=0;i<dw.size();i++) { dw[i] = (real)0;}
	}
	
//	var<Vol> cloneAndZero() const
//	{
//		var<Vol> v;
//		v = new Vol(sx,sy,depth,false,0);
//		return v;
//	}
//
//	var<Vol> clone() const
//	{
//		var<Vol> v;
//		v = new Vol(sx,sy,depth,false,0);
//		size_t i=0;
//		for(i=0;i<w.size();i++)
//		{
//			v->w[i] = w[i];
//			v->dw[i] = dw[i];
//		}
//		return v;
//	}

	void add_from(const Vol& _v)
	{
		size_t i=0;
		for(i=0;i<w.size();i++)
		{
			w[i] += _v.w[i];
		}
	}

	void add_from_scaled(const Vol& _v,real _a)
	{
		size_t i=0;
		for(i=0;i<w.size();i++)
		{
			w[i] += _v.w[i]*_a;
		}
	}

	void set_const(real _v)
	{
		size_t i=0;
		for(i=0;i<w.size();i++)
		{
			w[i] = _v;
		}
	}

	void regression(const Vol& _y) 
	{
		VERIFY_EXIT(_y.w.size()==dw.size() && _y.w.size()==w.size());
		size_t i=0;
		for(i=0;i<w.size();i++)
		{
			dw[i] = w[i] - _y.w[i];
		}
	}

};//struct Vol

inline
void img_to_vol(IN MemoryDC& _img,OUT Vol& _imgx,IN bool _bconvert2grayscale = false)
{
	_imgx.alloc(_img.getWidth(),_img.getHeight(),_bconvert2grayscale?1:4,false);
	long x,y;
	for(y=0;y<_img.getHeight();y++)
	for(x=0;x<_img.getWidth();x++)
	{
		BYTE r = 0,g = 0,b = 0,a = 0;
		_img.GetRGBA(x,y,r,g,b,a);
		if(!_bconvert2grayscale)
		{
			_imgx.set(x,y,0,r/(real)256.0-(real)0.5);
			_imgx.set(x,y,1,g/(real)256.0-(real)0.5);
			_imgx.set(x,y,2,b/(real)256.0-(real)0.5);
			_imgx.set(x,y,3,a/(real)256.0-(real)0.5);
		}
		else
		{
			_imgx.set(x,y,0,(real)RGBtoGray(r,g,b,a)/(real)256.0-(real)0.5);
		}
	}
}

enum LayerTypeEn
{
	layertype_null = 0
	,layertype_input 
	,layertype_fc
	,layertype_relu
	,layertype_sigmoid
	,layertype_regression
	,layertype_conv
	,layertype_pool
	,layertype_softmax
	,layertype_maxout
	,layertype_dropout
	,layertype_lrn
	,layertype_quadtransform
	,layertype_tanh
	,layertype_hidden_input 
	,layertype_hidden_regression
	,layertype_functional
};

struct Layer;

struct LayerOpts
{
	LayerTypeEn type;
	LayerTypeEn activation;
	size_t num_neurons;
	real l1_decay_mul;
	real l2_decay_mul;
	size_t in_sx,in_sy,in_depth;
	size_t out_sx,out_sy,out_depth;
	size_t extra_sx,extra_sy,extra_depth;
	size_t extra_offs;
	real bias_pref;
	bool b_bias_pref;
	size_t sx,sy;
	size_t filters;
	size_t stride;
	long pad;
	size_t num_inputs;
	size_t group_size;
	real drop_prob;
	bool b_drop_prob;
	real k;
	long n;
	real alpha;
	real beta;
	size_t num_classes;
	bool tensor;
	Layer* original;
	size_t identity;
	size_t start_drop;
	size_t stop_drop;
	size_t start_regression;
	size_t stop_regression;


	LayerOpts()
		:type(layertype_null)
		,activation(layertype_null)
		,num_neurons(0)
		,l1_decay_mul(0)
		,l2_decay_mul(0)
		,in_sx(0),in_sy(0),in_depth(0)
		,out_sx(0),out_sy(0),out_depth(0)
		,extra_sx(0),extra_sy(0),extra_depth(0)
		,extra_offs(0)
		,bias_pref((real)0.1)
		,b_bias_pref(false)
		,sx(0),sy(0)
		,filters(0)
		,stride(0)
		,pad(0)
		,num_inputs(0)
		,group_size(0)
		,drop_prob((real)0.5)
		,k(0)
		,n(0)
		,alpha(0)
		,beta(0)
		,num_classes(0)
		,tensor(false)
		,b_drop_prob(false)
		,original(NULL)
		,identity(0)
		,start_drop(0)
		,stop_drop(0)
		,start_regression(0)
		,stop_regression(0)
	{
	}

	void set_bias_pref(real _bias_pref)
	{
		b_bias_pref = true;
		bias_pref = _bias_pref;
	}

	void set_drop_params(real _drop_prob,size_t _start_drop,size_t _stop_drop)
	{
		b_drop_prob = true;	
		drop_prob = _drop_prob;
		start_drop = _start_drop;
		stop_drop = _stop_drop;
	}
};

struct ParamsAndGrads
{
	Vol::WT* params;
	Vol::WT* grads;
	real l1_decay_mul;
	real l2_decay_mul;

	ParamsAndGrads(
		Vol::WT* _params = NULL
		,Vol::WT* _grads = NULL
		,real _l1_decay_mul = (real)0.0
		,real _l2_decay_mul = (real)0.0
		)
		:params(_params)
		,grads(_grads)
		,l1_decay_mul(_l1_decay_mul)
		,l2_decay_mul(_l2_decay_mul)
	{
	}
};

typedef std::vector<ParamsAndGrads> ParamsAndGradsVec;

struct Layer
{
	size_t out_sx;
	size_t out_sy;
	size_t out_depth;
	size_t num_inputs;
	Vol* in_act;
	Vol out_act;
	bool stop_learning;
	bool start_learning;
	size_t identity;

	LayerTypeEn layer_type;

//	Layer()
//		:out_sx(0)
//		,out_sy(0)
//		,out_depth(0)
//		,num_inputs(0)
//		,layer_type(layertype_null)
//		,in_act(NULL)
//		,stop_learning(false)
//		,start_learning(false)
//		,identity(0)
//	{
//	}

	Layer(const LayerOpts& _opt)
		:out_sx(0)
		,out_sy(0)
		,out_depth(0)
		,num_inputs(0)
		,layer_type(layertype_null)
		,in_act(NULL)
		,stop_learning(false)
		,start_learning(false)
		,identity(_opt.identity)
	{
	}

	virtual ~Layer()
	{
		out_act.clear();
	}
	

	Layer(const Layer& _layer)
		:in_act(NULL)
		,identity(_layer.identity)
	{
		out_sx = _layer.out_sx;
		out_sy = _layer.out_sy;
		out_depth = _layer.out_depth;
		num_inputs = _layer.num_inputs;
		layer_type = _layer.layer_type;
	}

	Vol& get_input() {return out_act;}
	Vol& get_result() {return out_act;}

	virtual void initialize(const LayerOpts& _opt) = 0;
	virtual void forward(bool _is_training) {};
	virtual void backward(bool _blearn) {}
	virtual real backward(const Vol& _y,bool _blearn) {return 0;};
	virtual void getParamsAndGrads(ParamsAndGradsVec& _response) {_response.clear();}
	virtual void setParamsAndGrads(ParamsAndGradsVec& _response) {}
	virtual Layer* clone() const = 0;
	virtual void set_input(Vol* _input) {};
};

/*
// This file contains all layers that do dot products with input,
// but usually in a different connectivity pattern and weight sharing
// schemes: 
// - FullyConn is fully connected dot products 
// - ConvLayer does convolutions (so weight sharing spatially)
// putting them together in one file because they are very similar
var ConvLayer = function(opt) {
	var opt = opt || {};

	// required
	this.out_depth = opt.filters;
	this.sx = opt.sx; // filter size. Should be odd if possible, it's cleaner.
	this.in_depth = opt.in_depth;
	this.in_sx = opt.in_sx;
	this.in_sy = opt.in_sy;

	// optional
	this.sy = typeof opt.sy !== 'undefined' ? opt.sy : this.sx;
	this.stride = typeof opt.stride !== 'undefined' ? opt.stride : 1; // stride at which we apply filters to input volume
	this.pad = typeof opt.pad !== 'undefined' ? opt.pad : 0; // amount of 0 padding to add around borders of input volume
	this.l1_decay_mul = typeof opt.l1_decay_mul !== 'undefined' ? opt.l1_decay_mul : 0.0;
	this.l2_decay_mul = typeof opt.l2_decay_mul !== 'undefined' ? opt.l2_decay_mul : 1.0;

	// computed
	// note we are doing floor, so if the strided convolution of the filter doesnt fit into the input
	// volume exactly, the output volume will be trimmed and not contain the (incomplete) computed
	// final application.
	this.out_sx = Math.floor((this.in_sx + this.pad * 2 - this.sx) / this.stride + 1);
	this.out_sy = Math.floor((this.in_sy + this.pad * 2 - this.sy) / this.stride + 1);
	this.layer_type = 'conv';

	// initializations
	this.filters = [];
	for(var i=0;i<this.out_depth;i++) { this.filters.push(new Vol(this.sx, this.sy, this.in_depth)); }
	this.biases = new Vol(1, 1, this.out_depth, 0.1);
} 
*/
struct ConvLayer : public Layer
{
	size_t sx,sy,stride;
	size_t in_depth,in_sx,in_sy;
	long pad;
	real l1_decay_mul,l2_decay_mul;
	std::vector<Vol> filters;
	std::vector<Vol> biases;

	static void init(LayerOpts& _opt)
	{
		_opt.stride = 1;
		_opt.l1_decay_mul = (real)0.0;
		_opt.l2_decay_mul = (real)1.0;
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new ConvLayer(*this));
	}

	ConvLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,sx(0)
		,sy(0)
		,stride(0)
		,pad(0)
		,in_depth(0)
		,in_sx(0)
		,in_sy(0)
		,l1_decay_mul(0)
		,l2_decay_mul(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_depth = _opt.filters;
		sx = _opt.sx;
		sy = _opt.sy;
		in_depth = _opt.in_depth;
		in_sx = _opt.in_sx;
		in_sy = _opt.in_sy;
		stride = _opt.stride;
		pad = _opt.pad;
		l1_decay_mul = _opt.l1_decay_mul;
		l2_decay_mul = _opt.l2_decay_mul;

		out_sx = (in_sx + pad*2 - sx)/stride + 1;
		out_sy = (in_sy + pad*2 - sy)/stride + 1;
		layer_type = layertype_conv;
		filters.resize(out_depth);
		size_t i=0;
		for(i=0;i<out_depth;i++)
			filters[i].alloc(sx,sy,in_depth,true);
		biases.resize(1);
		biases[0].alloc(1,1,out_depth,false,(real)0.1);
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	ConvLayer(const ConvLayer& _cl)
		:Layer(_cl)
	{
		sx = _cl.sx;
		sy = _cl.sy;
		stride = _cl.stride;
		pad = _cl.pad;
		in_depth = _cl.in_depth;
		in_sx = _cl.in_sx;
		in_sy = _cl.in_sy;
		l1_decay_mul = _cl.l1_decay_mul;
		l2_decay_mul = _cl.l2_decay_mul;
		filters.resize(out_depth);
		size_t i=0;
		for(i=0;i<out_depth;i++)
			filters[i].alloc_copy(_cl.filters[i]);
		biases.resize(1);
		biases[0].alloc_copy(_cl.biases[0]);
		out_act.alloc_copy(_cl.out_act);
	}

	/*ConvLayer.prototype = {
	forward: function(V, is_training) {
		this.in_act = V;

		var A = new Vol(this.out_sx, this.out_sy, this.out_depth, 0.0);
		for(var d=0;d<this.out_depth;d++) {
			var f = this.filters[d];
			var x = -this.pad;
			var y = -this.pad;
			for(var ax=0; ax<this.out_sx; x+=this.stride,ax++) {
				y = -this.pad;
				for(var ay=0; ay<this.out_sy; y+=this.stride,ay++) {

					// convolve centered at this particular location
					// could be bit more efficient, going for correctness first
					var a = 0.0;
					for(var fx=0;fx<f.sx;fx++) {
						for(var fy=0;fy<f.sy;fy++) {
							for(var fd=0;fd<f.depth;fd++) {
								var oy = y+fy; // coordinates in the original input array coordinates
								var ox = x+fx;
								if(oy>=0 && oy<V.sy && ox>=0 && ox<V.sx) {
									//a += f.get(fx, fy, fd) * V.get(ox, oy, fd);
									// avoid function call overhead for efficiency, compromise modularity :(
									a += f.w[((f.sx * fy)+fx)*f.depth+fd] * V.w[((V.sx * oy)+ox)*V.depth+fd];
								}
							}
						}
					}
					a += this.biases.w[d];
					A.set(ax, ay, d, a);
				}
			}
		}
		this.out_act = A;
		return this.out_act;
	},*/

	void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& A = out_act;
		size_t d=0;
		for(d=0;d<out_depth;d++)
		{
			Vol& f = filters[d];
			long x = -pad;
			long y = -pad;
			long ax=0;
			for(ax=0;ax<(long)out_sx;x+=(long)stride,ax++)
			{
				y = -pad;
				long ay = 0;
				for(ay=0;ay<(long)out_sy;y+=(long)stride,ay++)
				{
					real a = 0;
					long fx=0,fy=0,fd=0;
					for(fy=0;fy<(long)f.sy;fy++)
					for(fx=0;fx<(long)f.sx;fx++)
					for(fd=0;fd<(long)f.depth;fd++)
					{
						long oy = y + fy;
						long ox = x + fx;
						if(oy>=0 && oy<(long)V.sy && ox>=0 && ox<(long)V.sx)
						{
							// a += f->get(fx, fy, fd) * _V->get(ox, oy, fd);
							a += 
								f.w[(((long)f.sx*fy)+fx)*(long)f.depth + fd]
								* V.w[(((long)V.sx * oy)+ox)*(long)V.depth+fd]
								;

						}
					}
					a += biases[0].w[d];
					A.set(ax,ay,d,a);
				}
			}
		}
	}
	/*backward: function()
	{ 
		// compute gradient wrt weights, biases and input data
		var V = this.in_act;
		V.dw = global.zeros(V.w.length); // zero out gradient wrt bottom data, we're about to fill it
		for(var d=0;d<this.out_depth;d++) {
			var f = this.filters[d];
			var x = -this.pad;
			var y = -this.pad;
			for(var ax=0; ax<this.out_sx; x+=this.stride,ax++) {
				y = -this.pad;
				for(var ay=0; ay<this.out_sy; y+=this.stride,ay++) {
					// convolve and add up the gradients. 
					// could be more efficient, going for correctness first
					var chain_grad = this.out_act.get_grad(ax,ay,d); // gradient from above, from chain rule
					for(var fx=0;fx<f.sx;fx++) {
						for(var fy=0;fy<f.sy;fy++) {
							for(var fd=0;fd<f.depth;fd++) {
								var oy = y+fy;
								var ox = x+fx;
								if(oy>=0 && oy<V.sy && ox>=0 && ox<V.sx) {
									// forward prop calculated: a += f.get(fx, fy, fd) * V.get(ox, oy, fd);
									//f.add_grad(fx, fy, fd, V.get(ox, oy, fd) * chain_grad);
									//V.add_grad(ox, oy, fd, f.get(fx, fy, fd) * chain_grad);

									// avoid function call overhead and use Vols directly for efficiency
									var ix1 = ((V.sx * oy)+ox)*V.depth+fd;
									var ix2 = ((f.sx * fy)+fx)*f.depth+fd;
									f.dw[ix2] += V.w[ix1]*chain_grad;
									V.dw[ix1] += f.w[ix2]*chain_grad;
								}
							}
						}
					}
					this.biases.dw[d] += chain_grad;
				}
			}
		}
	},*/
	void backward(bool _blearn)
	{
		Vol& V = *in_act;
		V.zero_dw();
		size_t d=0;
		for(d=0;d<out_depth;d++)
		{
			Vol& f = filters[d];
			long x = -pad;
			long y = -pad;
			long ax=0,ay=0,fx=0,fy=0,fd=0;
			for(ax=0;ax<(long)out_sx;x+=(long)stride,ax++)
			{
				y = -pad;
				for(ay=0;ay<(long)out_sy;y+=(long)stride,ay++)
				{
					real chain_grad = out_act.get_grad(ax,ay,d);
					for(fy=0;fy<(long)f.sy;fy++)
					for(fx=0;fx<(long)f.sx;fx++)
					for(fd=0;fd<(long)f.depth;fd++)
					{
						long oy = y + fy;
						long ox = x + fx;
						if(oy>=0 && oy<(long)V.sy && ox>=0 && ox<(long)V.sx)
						{
							long ix1 = (((long)V.sx*oy) + ox)*(long)V.depth + fd;
							long ix2 = (((long)f.sx*fy) + fx)*(long)f.depth + fd;
							if(_blearn)
								f.dw[ix2] += V.w[ix1]*chain_grad;
							V.dw[ix1] += f.w[ix2]*chain_grad;
						}
					}
					if(_blearn)
						biases[0].dw[d] += chain_grad;
				}
			}
		}
	}
	/*getParamsAndGrads: function()
	{
		var response = [];
		for(var i=0;i<this.out_depth;i++) {
			response.push({params: this.filters[i].w, grads: this.filters[i].dw, l2_decay_mul: this.l2_decay_mul, l1_decay_mul: this.l1_decay_mul});
		}
		response.push({params: this.biases.w, grads: this.biases.dw, l1_decay_mul: 0.0, l2_decay_mul: 0.0});
		return response;
	},*/

	void getParamsAndGrads(ParamsAndGradsVec& _response)
	{
		_response.clear();
		_response.reserve(out_depth+1);
		size_t i=0;
		for(i=0;i<out_depth;i++)
		{
			_response.push_back(ParamsAndGrads(&(filters[i].w),&(filters[i].dw),l1_decay_mul,l2_decay_mul));
		}
		_response.push_back(ParamsAndGrads(&(biases[0].w),&(biases[0].dw),0,0));
	}

	virtual void setParamsAndGrads(ParamsAndGradsVec& _response) 
	{
		size_t i=0,j=0;
		for(i=0;i<out_depth;i++)
		{
			VERIFY_DO(_response[i].params->size()==filters[i].w.size(),continue);
			for(j=0;j<filters[i].w.size();j++)
			{
				filters[i].w[j] = (*_response[i].params)[j];
			}
		}
		VERIFY_EXIT(_response[i].params->size()==biases[0].w.size());
		for(j=0;j<_response[i].params->size();j++)
			biases[0].w[j] = (*_response[i].params)[j];
	}
};//struct ConvLayer

struct FullyConnLayer : public Layer
{
	real l1_decay_mul;
	real l2_decay_mul;
	real bias;
	std::vector<Vol> filters;
	std::vector<Vol> biases;

	virtual Layer* clone() const
	{
		FullyConnLayer* layer = trace_alloc(new FullyConnLayer(*this));
		return layer;
	}

	static void init(LayerOpts& _opt)
	{
		_opt.l1_decay_mul = (real)0.0;
		_opt.l2_decay_mul = (real)1.0;
		_opt.bias_pref = (real)0.1;
	}

	FullyConnLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,l1_decay_mul(0)
		,l2_decay_mul(0)
		,bias(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_depth = _opt.num_neurons;
		l1_decay_mul = _opt.l1_decay_mul;
		l2_decay_mul = _opt.l2_decay_mul;
		num_inputs = _opt.in_sx*_opt.in_sy*_opt.in_depth;
		out_sx = 1;
		out_sy = 1;
		layer_type = layertype_fc;
		bias = _opt.b_bias_pref?_opt.bias_pref:(real)0.1;
		size_t i=0;
		filters.resize(out_depth);
		for(i=0;i<out_depth;i++)
			filters[i].alloc(1,1,num_inputs,true);
		biases.resize(1);
		biases[0].alloc(1,1,out_depth,false,bias);
		out_act.alloc(1,1,out_depth,false,(real)0.0);
	}

	FullyConnLayer(const FullyConnLayer& _fc)
		:Layer(_fc)
	{
		l2_decay_mul = _fc.l2_decay_mul;
		l1_decay_mul = _fc.l1_decay_mul;
		size_t i=0;
		filters.resize(out_depth);
		for(i=0;i<out_depth;i++)
			filters[i].alloc_copy(_fc.filters[i]);
		biases.resize(1);
		biases[0].alloc_copy(_fc.biases[0]);
		out_act.alloc_copy(_fc.out_act);
	}

	void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& A = out_act;
		const Vol::WT& Vw = V.w; 
		size_t i = 0;
		for(i=0;i<out_depth;i++)
		{
			real a = 0.0;
			Vol::WT& wi = filters[i].w;
			size_t d=0;
			for(d=0;d<num_inputs;d++)
			{
				a += Vw[d]*wi[d];
			}
			a += biases.front().w[i];
			A.w[i] = a;
		}
	}

	void backward(bool _blearn)
	{
		Vol& V = *in_act;
		V.zero_dw();

		size_t i=0;
		for(i=0;i<out_depth;i++)
		{
			Vol& tfi = filters[i];
			real chain_grad = out_act.dw[i];
			size_t d=0;
			for(d=0;d<num_inputs;d++)
			{
				V.dw[d] += tfi.w[d]*chain_grad;
				if(_blearn)
					tfi.dw[d] += V.w[d]*chain_grad;
			}
			if(_blearn)
				biases[0].dw[i] += chain_grad;
		}
	}

	/*void backward_temp()
	{
		Vol& V = *in_act;
		size_t d=0,i=0;
		for(d=0;d<num_inputs;d++)
		{
			real Vdwd = 0;
			real Vwd = V.w[d];
			for(i=0;i<out_depth;i++)
			{
				Vol& tfi = filters[i];
				real chain_grad = out_act.dw[i];
				Vdwd += tfi.w[d]*chain_grad;
				tfi.dw[d] += Vwd*chain_grad;
			}
			V.dw[d] = Vdwd;
		}
	}*/

	void getParamsAndGrads(ParamsAndGradsVec& _response)
	{
		_response.clear();
		_response.reserve(out_depth+1);
		size_t i=0;
		for(i=0;i<out_depth;i++)
		{
			_response.push_back(ParamsAndGrads(&(filters[i].w),&(filters[i].dw),l1_decay_mul,l2_decay_mul));
		}
		_response.push_back(ParamsAndGrads(&(biases[0].w),&(biases[0].dw),0,0));
	}

	virtual void setParamsAndGrads(ParamsAndGradsVec& _response) 
	{
		size_t i=0,j=0;
		for(i=0;i<out_depth;i++)
		{
			VERIFY_DO(_response[i].params->size()==filters[i].w.size(),continue);
			for(j=0;j<filters[i].w.size();j++)
			{
				filters[i].w[j] = (*_response[i].params)[j];
			}
		}
		VERIFY_EXIT(_response[i].params->size()==biases[0].w.size());
		for(j=0;j<_response[i].params->size();j++)
			biases[0].w[j] = (*_response[i].params)[j];
	}
};

/*var PoolLayer = function(opt) {

	var opt = opt || {};

	// required
	this.sx = opt.sx; // filter size
	this.in_depth = opt.in_depth;
	this.in_sx = opt.in_sx;
	this.in_sy = opt.in_sy;

	// optional
	this.sy = typeof opt.sy !== 'undefined' ? opt.sy : this.sx;
	this.stride = typeof opt.stride !== 'undefined' ? opt.stride : 2;
	this.pad = typeof opt.pad !== 'undefined' ? opt.pad : 0; // amount of 0 padding to add around borders of input volume

	// computed
	this.out_depth = this.in_depth;
	this.out_sx = Math.floor((this.in_sx + this.pad * 2 - this.sx) / this.stride + 1);
	this.out_sy = Math.floor((this.in_sy + this.pad * 2 - this.sy) / this.stride + 1);
	this.layer_type = 'pool';
	// store switches for x,y coordinates for where the max comes from, for each output neuron
	this.switchx = global.zeros(this.out_sx*this.out_sy*this.out_depth);
	this.switchy = global.zeros(this.out_sx*this.out_sy*this.out_depth);
}*/

struct PoolLayer : public Layer
{
	size_t in_sx,in_sy,in_depth;
	size_t sx,sy;
	long stride,pad;
	std::vector<long> switchx,switchy;

	static void init(LayerOpts& _opt)
	{
		_opt.stride = 2;
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new PoolLayer(*this));
	}

	PoolLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,sx(0),sy(0)
		,stride(0),pad(0)
		,in_sx(0),in_sy(0),in_depth(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		sx = _opt.sx;
		sy = _opt.sy;
		in_depth = _opt.in_depth;
		in_sx = _opt.in_sx;
		in_sy = _opt.in_sy;
		stride = _opt.stride;
		pad = _opt.pad;

		out_depth = in_depth;
		out_sx = (in_sx + pad*2 - sx)/stride  + 1;
		out_sy = (in_sy + pad*2 - sy)/stride  + 1;
		layer_type = layertype_pool;
		switchx.resize(out_sx*out_sy*out_depth);
		switchy.resize(out_sx*out_sy*out_depth);
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	PoolLayer(const PoolLayer& _pl)
		:Layer(_pl)
	{
		sx = _pl.sx;
		sy = _pl.sy;
		stride = _pl.stride;
		pad = _pl.pad;
		in_sx = _pl.in_sx;
		in_sy = _pl.in_sy;
		in_depth = _pl.in_depth;
		switchx.resize(out_sx*out_sy*out_depth);
		switchy.resize(out_sx*out_sy*out_depth);
		out_act.alloc_copy(_pl.out_act);
	}

	/*forward: function(V, is_training)
	{
		this.in_act = V;

		var A = new Vol(this.out_sx, this.out_sy, this.out_depth, 0.0);

		var n=0; // a counter for switches
		for(var d=0;d<this.out_depth;d++) {
			var x = -this.pad;
			var y = -this.pad;
			for(var ax=0; ax<this.out_sx; x+=this.stride,ax++) {
				y = -this.pad;
				for(var ay=0; ay<this.out_sy; y+=this.stride,ay++) {

					// convolve centered at this particular location
					var a = -99999; // hopefully small enough ;\
					var winx=-1,winy=-1;
					for(var fx=0;fx<this.sx;fx++) {
						for(var fy=0;fy<this.sy;fy++) {
							var oy = y+fy;
							var ox = x+fx;
							if(oy>=0 && oy<V.sy && ox>=0 && ox<V.sx) {
								var v = V.get(ox, oy, d);
								// perform max pooling and store pointers to where
								// the max came from. This will speed up backprop 
								// and can help make nice visualizations in future
								if(v > a) { a = v; winx=ox; winy=oy;}
							}
						}
					}
					this.switchx[n] = winx;
					this.switchy[n] = winy;
					n++;
					A.set(ax, ay, d, a);
				}
			}
		}
		this.out_act = A;
		return this.out_act;
	},*/

	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& A = out_act;
		size_t n=0,d=0;
		long ax=0,ay=0,fx=0,fy=0;
		for(d=0,n=0;d<out_depth;d++)
		{
			long x = -pad;
			long y = -pad;
			for(ax=0;ax<(long)out_sx;x+=stride,ax++)
			{
				y = -pad;
				for(ay=0;ay<(long)out_sy;y+=stride,ay++)
				{
					real a = -99999; 
					long winx=-1,winy=-1;
					for(fy=0;fy<(long)sy;fy++)
					for(fx=0;fx<(long)sx;fx++)
					{
						long oy = y + fy;
						long ox = x + fx;
						if(oy>=0 && oy<(long)V.sy && ox>=0 && ox<(long)V.sx)
						{
							real v = V.get(ox,oy,d);
							if(v>a){a=v;winx=ox,winy=oy;}
						}
					}
					switchx[n] = winx;
					switchy[n] = winy;
					n++;
					A.set(ax,ay,d,a);
				}
			}
		}
	}

	/*backward: function()
	{ 
		// pooling layers have no parameters, so simply compute 
		// gradient wrt data here
		var V = this.in_act;
		V.dw = global.zeros(V.w.length); // zero out gradient wrt data
		var A = this.out_act; // computed in forward pass 

		var n = 0;
		for(var d=0;d<this.out_depth;d++) {
			var x = -this.pad;
			var y = -this.pad;
			for(var ax=0; ax<this.out_sx; x+=this.stride,ax++) {
				y = -this.pad;
				for(var ay=0; ay<this.out_sy; y+=this.stride,ay++) {

					var chain_grad = this.out_act.get_grad(ax,ay,d);
					V.add_grad(this.switchx[n], this.switchy[n], d, chain_grad);
					n++;

				}
			}
		}
	},*/
	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		V.zero_dw();
		Vol& A = out_act;
		size_t n=0;
		long d=0,ax=0,ay=0;
		for(n=0,d=0;d<(long)out_depth;d++)
		{
			long x = -pad;
			long y = -pad;
			for(ay=0;ay<(long)out_sy;y+=stride,ay++)
			{
				x = -pad;
				for(ax=0;ax<(long)out_sx;x+=stride,ax++)
				{
					real chain_grad = out_act.get_grad(ax,ay,d);
					V.add_grad(switchx[n],switchy[n],d,chain_grad);
					n++;
				}
			}
		}
	}
};//struct PoolLayer

struct InputLayer : public Layer
{
	size_t extra_sx,extra_sy,extra_depth;

	static void init(LayerOpts& _opt)
	{
	}

	InputLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,extra_sx(0),extra_sy(0),extra_depth(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_sx = _opt.out_sx!=0?_opt.out_sx:_opt.in_sx;
		out_sy = _opt.out_sy!=0?_opt.out_sy:_opt.in_sy;
		out_depth = _opt.out_depth!=0?_opt.out_depth:_opt.in_depth;
		extra_sx = _opt.extra_sx;
		extra_sy = _opt.extra_sy;
		extra_depth = _opt.extra_depth;
		layer_type = layertype_input;
		out_act.alloc(out_sx+extra_sx,out_sy+extra_sy,out_depth+extra_depth,false);
	}

	InputLayer(const InputLayer& _layer)
		:Layer(_layer)
		,extra_sx(_layer.extra_sx)
		,extra_sy(_layer.extra_sy)
		,extra_depth(_layer.extra_depth)
	{
		out_act.alloc_copy(_layer.out_act);
	}

	virtual Layer* clone() const
	{
		InputLayer* layer = trace_alloc(new InputLayer(*this));
		return layer;
	}
};

/*
// This is a classifier, with N discrete classes from 0 to N-1
// it gets a stream of N incoming numbers and computes the softmax
// function (exponentiate and normalize to sum to 1 as probabilities should)
var SoftmaxLayer = function(opt) {
	var opt = opt || {};

	// computed
	this.num_inputs = opt.in_sx * opt.in_sy * opt.in_depth;
	this.out_depth = this.num_inputs;
	this.out_sx = 1;
	this.out_sy = 1;
	this.layer_type = 'softmax';
}*/

struct SoftmaxLayer : public Layer
{
	Vol::WT es;

	static void init(LayerOpts& _opt)
	{
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new SoftmaxLayer(*this));
	}

	SoftmaxLayer(const LayerOpts& _opt)
		:Layer(_opt)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		num_inputs = _opt.in_sx*_opt.in_sy*_opt.in_depth;
		out_depth = num_inputs;
		out_sx = 1;
		out_sy = 1;
		layer_type = layertype_softmax;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	SoftmaxLayer(const SoftmaxLayer& _sl)
		:Layer(_sl)
	{
		out_act.alloc_copy(_sl.out_act);
	}

	/*SoftmaxLayer.prototype = {
	forward: function(V, is_training)
	{
		this.in_act = V;

		var A = new Vol(1, 1, this.out_depth, 0.0);

		// compute max activation
		var as = V.w;
		var amax = V.w[0];
		for(var i=1;i<this.out_depth;i++) {
			if(as[i] > amax) amax = as[i];
		}

		// compute exponentials (carefully to not blow up)
		var es = global.zeros(this.out_depth);
		var esum = 0.0;
		for(var i=0;i<this.out_depth;i++) {
			var e = Math.exp(as[i] - amax);
			esum += e;
			es[i] = e;
		}

		// normalize and output to sum to one
		for(var i=0;i<this.out_depth;i++) {
			es[i] /= esum;
			A.w[i] = es[i];
		}

		this.es = es; // save these for backprop
		this.out_act = A;
		return this.out_act;
	},*/

	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& A = out_act;
		const Vol::WT& as = V.w;
		real amax = V.w[0];
		size_t i=0;
		for(i=0;i<out_depth;i++)
		{
			if(as[i]>amax) amax = as[i];
		}
		es.resize(out_depth);
		real esum = 0.0;
		for(i=0;i<out_depth;i++)
		{
			real e = exp(as[i]-amax);
			esum += e;
			es[i] = e;
		}
		for(i=0;i<out_depth;i++)
		{
			es[i] /= esum;
			A.w[i] = es[i];
		}
	}

	/*backward: function(y)
	{

		// compute and accumulate gradient wrt weights and bias of this layer
		var x = this.in_act;
		x.dw = global.zeros(x.w.length); // zero out the gradient of input Vol

		for(var i=0;i<this.out_depth;i++) {
			var indicator = i === y ? 1.0 : 0.0;
			var mul = -(indicator - this.es[i]);
			x.dw[i] = mul;
		}

		// loss is the class negative log likelihood
		return -Math.log(this.es[y]);
	},*/

	virtual real backward(const Vol& _y,bool _blearn)
	{
		Vol& x = *in_act;
		x.zero_dw();
		size_t i=0;
		size_t y = 0;
		for(i=0;i<out_depth;i++)
		{
			if(_y.w[i]>0.0) y=i;
			real mul = -(_y.w[i]+(real)0.5 - es[i]);
			x.dw[i] = mul;
		}
		return -log(es[y]);
	}
};//struct SoftmaxLayer


struct RegressionLayer : public Layer
{
	bool outter_dw;
	size_t start_regression;
	size_t stop_regression;

	static void init(LayerOpts& _opt)
	{
		_opt.start_regression = 0;
		_opt.stop_regression = 0;
	}

	RegressionLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,outter_dw(false)
		,start_regression(0)
		,stop_regression(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		num_inputs = _opt.in_sx*_opt.in_sy*_opt.in_depth;
		out_depth = num_inputs;
		out_sx = 1;
		out_sy = 1;
		layer_type = layertype_regression;
		outter_dw = false;
		start_regression = _opt.start_regression;
		stop_regression = _opt.stop_regression!=0?_opt.stop_regression:out_depth;
		out_act.alloc(out_sx,out_sy,out_depth,false,0.0);
	}

	RegressionLayer(const RegressionLayer& _layer)
		:Layer(_layer)
		,outter_dw(_layer.outter_dw)
	{
		out_act.alloc_copy(_layer.out_act);
	}

	virtual void forward(bool _is_training)
	{
		out_act.copy_w(*in_act);
	}

	virtual real backward(const Vol& _y,bool _blearn)
	{
		Vol& x = *in_act;
		if(!outter_dw)
		{
			real loss = 0;
			x.zero_dw();
			size_t i=0;
			for(i=start_regression;i<stop_regression;i++)
			{
				real dy = (x.w[i] - _y.w[i]);
				x.dw[i] = dy;
				loss += 2*dy*dy;
			}
			return loss;
		}
		else
		{
			x.copy_dw(out_act);
			return 0;
		}
	}

	virtual Layer* clone() const
	{
		RegressionLayer* layer = trace_alloc(new RegressionLayer(*this));
		return layer;
	}
};

struct ReluLayer : public Layer
{
	static void init(LayerOpts& _opt)
	{
	}

	ReluLayer(const LayerOpts& _opt)
		:Layer(_opt)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth;
		layer_type = layertype_relu;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	ReluLayer(const ReluLayer& _layer)
		:Layer(_layer)
	{
		out_act.alloc_copy(_layer.out_act);
	}

	virtual void forward(bool _is_training)
	{
		Vol& _V = *in_act;
		Vol& V2 = out_act;
		V2.copy(_V);
		Vol::WT& V2w = V2.w;
		size_t i=0;
		for(i=0;i<V2w.size();i++)
		{
			if(V2w[i]<0) V2w[i] = 0;
		}
	}

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		V.zero_dw();
		size_t i=0,N = V.w.size();
		for(i=0;i<N;i++)
		{
			if(V2.w[i]<=0) V.dw[i] = 0;
			else V.dw[i] = V2.dw[i];
		}
	}

	virtual Layer* clone() const
	{
		ReluLayer* layer = trace_alloc(new ReluLayer(*this));
		return layer;
	}
};

struct HiddenInput : public Layer
{
	size_t extra_offs;
	size_t extra_size;
	Vol* input;

	static void init(LayerOpts& _opt)
	{
	}

	HiddenInput(const LayerOpts& _opt)
		:Layer(_opt)
		,input(NULL)
		,extra_offs(0)
		,extra_size(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		extra_offs = _opt.extra_offs;
		extra_size = max(1,_opt.extra_sx)*max(1,_opt.extra_sy)*max(1,_opt.extra_depth);
		out_sx = _opt.in_sx + _opt.extra_sx;
		out_sy = _opt.in_sy + _opt.extra_sy;
		out_depth = _opt.in_depth + _opt.extra_depth;
		layer_type = layertype_hidden_input;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	virtual void set_input(Vol* _input)
	{
		input = _input;
	}

	HiddenInput(const HiddenInput& _layer)
		:Layer(_layer)
		,input(NULL)
		,extra_offs(_layer.extra_offs)
		,extra_size(_layer.extra_size)
	{
		out_act.alloc_copy(_layer.out_act);
	}

	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		size_t i=0;
		for(i=0;i<V.w.size();i++)
		{
			V2.w[i] = V.w[i];
		}
		size_t j=0;
		for(j=0;j<extra_size;j++,i++)
		{
			V2.w[i] = input->w[extra_offs+j];
		}
	}

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		V.zero_dw();
		size_t i=0,N = V.w.size();
		for(i=0;i<N;i++)
		{
			V.dw[i] = V2.dw[i];
		}
	}

	virtual Layer* clone() const
	{
		HiddenInput* layer = trace_alloc(new HiddenInput(*this));
		return layer;
	}
};//struct HiddenInput

struct HiddenRegression : public Layer
{
	size_t extra_offs;
	size_t extra_size;
	Vol* input;
	Vol loss;

	static void init(LayerOpts& _opt)
	{
	}

	HiddenRegression(const LayerOpts& _opt)
		:Layer(_opt)
		,input(NULL)
		,extra_offs(0)
		,extra_size(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		extra_offs = _opt.extra_offs;
		extra_size = max(1,_opt.extra_sx)*max(1,_opt.extra_sy)*max(1,_opt.extra_depth);
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth;
		layer_type = layertype_hidden_regression;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
		loss.alloc(max(1,_opt.extra_sx),max(1,_opt.extra_sy),max(1,_opt.extra_depth),false,(real)0.0);
	}

	virtual void set_input(Vol* _vol)
	{
		input = _vol;
	}

	HiddenRegression(const HiddenRegression& _layer)
		:Layer(_layer)
		,input(NULL)
		,extra_offs(_layer.extra_offs)
		,extra_size(_layer.extra_size)
	{
		out_act.alloc_copy(_layer.out_act);
		loss.alloc_copy(_layer.loss);
	}

	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		size_t i=0;
		for(i=0;i<V.w.size()-extra_size;i++)
		{
			V2.w[i] = V.w[i];
		}
		size_t j=0;
		for(j=0;j<extra_size;j++,i++)
		{
			V2.w[i] = input->w[extra_offs+j];
		}
	}

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		V.zero_dw();
		size_t i=0,N = V.w.size(),j=0;
		for(i=0;i<N-extra_size;i++)
		{
			V.dw[i] = V2.dw[i];
		}
		for(j=0;j<extra_size;j++,i++)
		{
			real dy = (V.w[i] - input->w[extra_offs+j]);
			loss.w[j] = dy;
			V.dw[i] = dy;
		}
	}

	virtual Layer* clone() const
	{
		HiddenRegression* layer = trace_alloc(new HiddenRegression(*this));
		return layer;
	}
};//struct HiddenInput

struct SigmoidLayer : public Layer
{
	static void init(LayerOpts& _opt)
	{
	}

	SigmoidLayer(const LayerOpts& _opt)
		:Layer(_opt)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth;
		layer_type = layertype_sigmoid;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	SigmoidLayer(const SigmoidLayer& _layer)
		:Layer(_layer)
	{
		out_act.alloc_copy(_layer.out_act);
	}

	virtual void forward(bool _is_training)
	{
		Vol& _V = *in_act;
		Vol& V2 = out_act;
		size_t N = _V.w.size();
		Vol::WT& V2w = V2.w;
		const Vol::WT& Vw = _V.w;
		size_t i=0;
		for(i=0;i<N;i++)
		{
			V2w[i] = (real)1.0/((real)1.0 + exp(-Vw[i]));
		}
	}

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		size_t N = V.w.size();
		V.zero_dw();
		size_t i=0;
		for(i=0;i<N;i++)
		{
			real v2wi = V2.w[i];
			V.dw[i] = v2wi*((real)1.0 - v2wi)*V2.dw[i];
		}
	}

	virtual Layer* clone() const
	{
		SigmoidLayer* layer = trace_alloc(new SigmoidLayer(*this));
		return layer;
	}
};

/*
// Implements Maxout nonlinearity that computes
// x -> max(x)
// where x is a vector of size group_size. Ideally of course,
// the input size should be exactly divisible by group_size
var MaxoutLayer = function(opt) {
	var opt = opt || {};

	// required
	this.group_size = typeof opt.group_size !== 'undefined' ? opt.group_size : 2;

	// computed
	this.out_sx = opt.in_sx;
	this.out_sy = opt.in_sy;
	this.out_depth = Math.floor(opt.in_depth / this.group_size);
	this.layer_type = 'maxout';

	this.switches = global.zeros(this.out_sx*this.out_sy*this.out_depth); // useful for backprop
}
*/

struct MaxoutLayer : public Layer
{
	size_t group_size;
	std::vector<long> switches;

	static void init(LayerOpts& _opt)
	{
		_opt.group_size = 2;
	}

	MaxoutLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,group_size(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		group_size = _opt.group_size;
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth/group_size;
		layer_type = layertype_maxout;
		switches.resize(out_sx*out_sy*out_depth);
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new MaxoutLayer(*this));
	}

	MaxoutLayer(const MaxoutLayer& _ml)
		:Layer(_ml)
	{
		group_size = _ml.group_size;
		switches.resize(out_sx*out_sy*out_depth);	
		out_act.alloc_copy(_ml.out_act);
	}

	/*MaxoutLayer.prototype = {
	forward: function(V, is_training)
	{
		this.in_act = V;
		var N = this.out_depth; 
		var V2 = new Vol(this.out_sx, this.out_sy, this.out_depth, 0.0);

		// optimization branch. If we're operating on 1D arrays we dont have
		// to worry about keeping track of x,y,d coordinates inside
		// input volumes. In convnets we do :(
		if(this.out_sx === 1 && this.out_sy === 1) {
			for(var i=0;i<N;i++) {
				var ix = i * this.group_size; // base index offset
				var a = V.w[ix];
				var ai = 0;
				for(var j=1;j<this.group_size;j++) {
					var a2 = V.w[ix+j];
					if(a2 > a) {
						a = a2;
						ai = j;
					}
				}
				V2.w[i] = a;
				this.switches[i] = ix + ai;
			}
		} else {
			var n=0; // counter for switches
			for(var x=0;x<V.sx;x++) {
				for(var y=0;y<V.sy;y++) {
					for(var i=0;i<N;i++) {
						var ix = i * this.group_size;
						var a = V.get(x, y, ix);
						var ai = 0;
						for(var j=1;j<this.group_size;j++) {
							var a2 = V.get(x, y, ix+j);
							if(a2 > a) {
								a = a2;
								ai = j;
							}
						}
						V2.set(x,y,i,a);
						this.switches[n] = ix + ai;
						n++;
					}
				}
			}

		}
		this.out_act = V2;
		return this.out_act;
	},*/
	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		size_t N = out_depth;
		Vol& V2 = out_act;
		if(out_sx==1 && out_sy==1)
		{
			size_t i=0;
			for(i=0;i<N;i++)
			{
				size_t ix = i*group_size;
				real a = V.w[ix];
				size_t ai = 0;
				size_t j=0;
				for(j=0;j<group_size && ix+j<N;j++)
				{
					real a2 = V.w[ix+j];
					if(a2>a)
					{
						a = a2;
						ai = j;
					}
				}
				V2.w[i] = a;
				switches[i] = ix+ai;
			}
		}
		else
		{
			size_t n=0;
			size_t x=0,y=0,i=0;
			for(y=0;y<V.sy;y++)
			for(x=0;x<V.sx;x++)
			for(i=0;i<N;i++)
			{
				size_t ix = i*group_size;
				real a = V.get(x,y,ix);
				size_t ai = 0;
				size_t j=0;
				for(j=0;j<group_size && ix+j<N;j++)
				{
					real a2 = V.get(x,y,ix+j);
					if(a2>a)
					{
						a = a2;
						ai = j;
					}
				}
				V2.set(x,y,i,a);
				switches[n] = ix + ai;
				n++;
			}
		}
	}
	/*backward: function()
	{
		var V = this.in_act; // we need to set dw of this
		var V2 = this.out_act;
		var N = this.out_depth;
		V.dw = global.zeros(V.w.length); // zero out gradient wrt data

		// pass the gradient through the appropriate switch
		if(this.out_sx === 1 && this.out_sy === 1) {
			for(var i=0;i<N;i++) {
				var chain_grad = V2.dw[i];
				V.dw[this.switches[i]] = chain_grad;
			}
		} else {
			// bleh okay, lets do this the hard way
			var n=0; // counter for switches
			for(var x=0;x<V2.sx;x++) {
				for(var y=0;y<V2.sy;y++) {
					for(var i=0;i<N;i++) {
						var chain_grad = V2.get_grad(x,y,i);
						V.set_grad(x,y,this.switches[n],chain_grad);
						n++;
					}
				}
			}
		}
	},*/

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		size_t N = out_depth;
		V.zero_dw();
		if(out_sx==1 && out_sy==1)
		{
			size_t i=0;
			for(i=0;i<N;i++)
			{
				real chain_grad = V2.dw[i];
				V.dw[switches[i]] = chain_grad;
			}
		}
		else
		{
			size_t n=0;
			size_t x=0,y=0,i=0;
			for(y=0;y<V2.sy;y++)
			for(x=0;x<V2.sx;x++)
			for(i=0;i<N;i++)
			{
				real chain_grad = V2.get_grad(x,y,i);
				V.set_grad(x,y,switches[n],chain_grad);
				n++;
			}
		}
	}
};//struct MaxoutLayer

/*
  // a helper function, since tanh is not yet part of ECMAScript. Will be in v6.
  function tanh(x) {
    var y = Math.exp(2 * x);
    return (y - 1) / (y + 1);
  }
  // Implements Tanh nonlinearity elementwise
  // x -> tanh(x) 
  // so the output is between -1 and 1.
  var TanhLayer = function(opt) {
    var opt = opt || {};

    // computed
    this.out_sx = opt.in_sx;
    this.out_sy = opt.in_sy;
    this.out_depth = opt.in_depth;
    this.layer_type = 'tanh';
  }
  TanhLayer.prototype = {
    forward: function(V, is_training) {
      this.in_act = V;
      var V2 = V.cloneAndZero();
      var N = V.w.length;
      for(var i=0;i<N;i++) { 
        V2.w[i] = tanh(V.w[i]);
      }
      this.out_act = V2;
      return this.out_act;
    },
    backward: function() {
      var V = this.in_act; // we need to set dw of this
      var V2 = this.out_act;
      var N = V.w.length;
      V.dw = global.zeros(N); // zero out gradient wrt data
      for(var i=0;i<N;i++) {
        var v2wi = V2.w[i];
        V.dw[i] = (1.0 - v2wi * v2wi) * V2.dw[i];
      }
    },
    getParamsAndGrads: function() {
      return [];
    },
    toJSON: function() {
      var json = {};
      json.out_depth = this.out_depth;
      json.out_sx = this.out_sx;
      json.out_sy = this.out_sy;
      json.layer_type = this.layer_type;
      return json;
    },
    fromJSON: function(json) {
      this.out_depth = json.out_depth;
      this.out_sx = json.out_sx;
      this.out_sy = json.out_sy;
      this.layer_type = json.layer_type; 
    }
  }
  
  global.TanhLayer = TanhLayer;
  global.MaxoutLayer = MaxoutLayer;
  global.ReluLayer = ReluLayer;
  global.SigmoidLayer = SigmoidLayer;

})(convnetjs);
*/

//inline
//real tanh(real _x)
//{
//	real y = exp(2*_x);
//	return (y - 1) / (y + 1);
//}

struct TanhLayer : public Layer
{
	static void init(LayerOpts& _opt)
	{
	}

	TanhLayer(const LayerOpts& _opt)
		:Layer(_opt)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth;
		layer_type = layertype_tanh;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	TanhLayer(const TanhLayer& _layer)
		:Layer(_layer)
	{
		out_act.alloc_copy(_layer.out_act);
	}

	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		size_t N = V.w.size();
		Vol::WT& V2w = V2.w;
		const Vol::WT& Vw = V.w;
		size_t i=0;
		for(i=0;i<N;i++)
		{
			V2w[i] = tanh(Vw[i]);
		}
	}

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& V2 = out_act;
		size_t N = V.w.size();
		V.zero_dw();
		size_t i=0;
		for(i=0;i<N;i++)
		{
			real v2wi = V2.w[i];
			V.dw[i] = ((real)1.0 - v2wi*v2wi)*V2.dw[i];
		}
	}

	virtual Layer* clone() const
	{
		TanhLayer* layer = trace_alloc(new TanhLayer(*this));
		return layer;
	}
};//struct TahnLayer

/*
// An inefficient dropout layer
// Note this is not most efficient implementation since the layer before
// computed all these activations and now we're just going to drop them :(
// same goes for backward pass. Also, if we wanted to be efficient at test time
// we could equivalently be clever and upscale during train and copy pointers during test
// todo: make more efficient.
var DropoutLayer = function(opt) {
	var opt = opt || {};

	// computed
	this.out_sx = opt.in_sx;
	this.out_sy = opt.in_sy;
	this.out_depth = opt.in_depth;
	this.layer_type = 'dropout';
	this.drop_prob = typeof opt.drop_prob !== 'undefined' ? opt.drop_prob : 0.5;
	this.dropped = global.zeros(this.out_sx*this.out_sy*this.out_depth);
}
*/

struct DropoutLayer : public Layer
{
	real drop_prob;
	std::vector<bool> dropped;
	size_t start_drop;
	size_t stop_drop;

	static void init(LayerOpts& _opt)
	{
		_opt.drop_prob = (real)0.5;
		_opt.start_drop = 0;
		_opt.stop_drop = 0;
	}

	DropoutLayer(const LayerOpts& _opt)
		:Layer(_opt)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth;
		layer_type = layertype_dropout;
		drop_prob = _opt.drop_prob;
		start_drop = _opt.start_drop;
		stop_drop = _opt.stop_drop!=0?_opt.stop_drop:out_sx*out_sy*out_depth;
		dropped.resize(out_sx*out_sy*out_depth);
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new DropoutLayer(*this));
	}

	DropoutLayer(const DropoutLayer& _dl)
		:Layer(_dl)
		,start_drop(_dl.start_drop)
		,stop_drop(_dl.stop_drop)
		,drop_prob(_dl.drop_prob)
	{
		dropped.resize(out_sx*out_sy*out_depth);
		out_act.alloc_copy(_dl.out_act);
	}

	/*DropoutLayer.prototype = {
	forward: function(V, is_training) 
	{
		this.in_act = V;
		if(typeof(is_training)==='undefined') { is_training = false; } // default is prediction mode
		var V2 = V.clone();
		var N = V.w.length;
		if(is_training) {
			// do dropout
			for(var i=0;i<N;i++) {
				if(Math.random()<this.drop_prob) { V2.w[i]=0; this.dropped[i] = true; } // drop!
				else {this.dropped[i] = false;}
			}
		} else {
			// scale the activations during prediction
			for(var i=0;i<N;i++) { V2.w[i]*=this.drop_prob; }
		}
		this.out_act = V2;
		return this.out_act; // dummy identity function for now
	},*/

	virtual void forward(bool _is_training)
	{
		Vol& _V = *in_act;
		out_act.copy(_V);
		Vol& V2 = out_act;
		size_t N = _V.w.size();
		if(_is_training)
		{
			// do dropout 
			size_t i=0;
			for(i=start_drop;i<stop_drop;i++)
			{
				if(::rand((real)1.0)<drop_prob) {V2.w[i] = 0; dropped[i] = true;} // drop!
				else {dropped[i] = false;}
			}
		}
		else
		{
			size_t i=0;
			for(i=0;i<N;i++) {V2.w[i]*=drop_prob;}
		}
	}

	/*backward: function()
	{
		var V = this.in_act; // we need to set dw of this
		var chain_grad = this.out_act;
		var N = V.w.length;
		V.dw = global.zeros(N); // zero out gradient wrt data
		for(var i=0;i<N;i++) {
			if(!(this.dropped[i])) { 
				V.dw[i] = chain_grad.dw[i]; // copy over the gradient
			}
		}
	},*/

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		Vol& chain_grad = out_act;
		size_t N = V.w.size();
		V.zero_dw();
		size_t i=0;
		for(i=start_drop;i<stop_drop;i++)
		{
			if(!dropped[i])
			{
				V.dw[i] = chain_grad.dw[i];
			}
		}
	}
};

/*
// a bit experimental layer for now. I think it works but I'm not 100%
// the gradient check is a bit funky. I'll look into this a bit later.
// Local Response Normalization in window, along depths of volumes
var LocalResponseNormalizationLayer = function(opt) {
	var opt = opt || {};

	// required
	this.k = opt.k;
	this.n = opt.n;
	this.alpha = opt.alpha;
	this.beta = opt.beta;

	// computed
	this.out_sx = opt.in_sx;
	this.out_sy = opt.in_sy;
	this.out_depth = opt.in_depth;
	this.layer_type = 'lrn';

	// checks
	if(this.n%2 === 0) { console.log('WARNING n should be odd for LRN layer'); }
}*/


struct LocalResponseNormalizationLayer : public Layer
{
	real k;
	long n;
	real alpha;
	real beta;
	Vol S_cache;

	static void init(LayerOpts& _opt)
	{
	}

	LocalResponseNormalizationLayer(const LayerOpts& _opt)
		:Layer(_opt)
		,k(0)
		,n(0)
		,alpha(0)
		,beta(0)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		k = _opt.k;
		n = _opt.n;
		alpha = _opt.alpha;
		beta = _opt.beta;

		out_sx = _opt.out_sx;
		out_sy = _opt.out_sy;
		out_depth = _opt.out_depth;
		layer_type = layertype_lrn;
		VERIFY_EXIT(n%2!=0);
		S_cache.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new LocalResponseNormalizationLayer(*this));
	}

	LocalResponseNormalizationLayer(const LocalResponseNormalizationLayer& _lrnl)
		:Layer(_lrnl)
	{
		k = _lrnl.k;
		n = _lrnl.n;
		alpha = _lrnl.alpha;
		beta = _lrnl.beta;
		S_cache.alloc_copy(_lrnl.S_cache);
		out_act.alloc_copy(_lrnl.out_act);
	}

	/*LocalResponseNormalizationLayer.prototype = {
	forward: function(V, is_training)
	{
		this.in_act = V;

		var A = V.cloneAndZero();
		this.S_cache_ = V.cloneAndZero();
		var n2 = Math.floor(this.n/2);
		for(var x=0;x<V.sx;x++) {
			for(var y=0;y<V.sy;y++) {
				for(var i=0;i<V.depth;i++) {

					var ai = V.get(x,y,i);

					// normalize in a window of size n
					var den = 0.0;
					for(var j=Math.max(0,i-n2);j<=Math.min(i+n2,V.depth-1);j++) {
						var aa = V.get(x,y,j);
						den += aa*aa;
					}
					den *= this.alpha / this.n;
					den += this.k;
					this.S_cache_.set(x,y,i,den); // will be useful for backprop
					den = Math.pow(den, this.beta);
					A.set(x,y,i,ai/den);
				}
			}
		}

		this.out_act = A;
		return this.out_act; // dummy identity function for now
	},*/

	virtual void forward(bool _is_training)
	{
		Vol& _V = *in_act;
		Vol& A = out_act;
		//S_cache = _V->cloneAndZero();
		long n2 = n/2;
		long x=0,y=0,i=0,j=0;
		for(y=0;y<(long)_V.sy;y++)
		for(x=0;x<(long)_V.sx;x++)
		for(i=0;i<(long)_V.depth;i++)
		{
			real ai = _V.get(x,y,i);
			real den = 0;
			for(j=max(0,i-n2);j<=min(i+n2,(long)_V.depth-1);j++)
			{
				real aa = _V.get(x,y,j);
				den += aa*aa;
			}
			den *= alpha/n;
			den += k;
			S_cache.set(x,y,i,den);
			den = pow(den,beta);
			A.set(x,y,i,ai/den);
		}
	}

	/*backward: function()
	{ 
		// evaluate gradient wrt data
		var V = this.in_act; // we need to set dw of this
		V.dw = global.zeros(V.w.length); // zero out gradient wrt data
		var A = this.out_act; // computed in forward pass 

		var n2 = Math.floor(this.n/2);
		for(var x=0;x<V.sx;x++) {
			for(var y=0;y<V.sy;y++) {
				for(var i=0;i<V.depth;i++) {

					var chain_grad = this.out_act.get_grad(x,y,i);
					var S = this.S_cache_.get(x,y,i);
					var SB = Math.pow(S, this.beta);
					var SB2 = SB*SB;

					// normalize in a window of size n
					for(var j=Math.max(0,i-n2);j<=Math.min(i+n2,V.depth-1);j++) {              
						var aj = V.get(x,y,j); 
						var g = -aj*this.beta*Math.pow(S,this.beta-1)*this.alpha/this.n*2*aj;
						if(j===i) g+= SB;
						g /= SB2;
						g *= chain_grad;
						V.add_grad(x,y,j,g);
					}

				}
			}
		}
	},*/

	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		V.zero_dw();
		Vol& A = out_act;

		long n2 = n/2;
		long x=0,y=0,i=0,j=0;
		for(y=0;y<(long)V.sy;y++)
		for(x=0;x<(long)V.sx;x++)
		for(i=0;i<(long)V.depth;i++)
		{
			real chain_grad = out_act.get_grad(x,y,i);
			real S = S_cache.get(x,y,i);
			real SB = pow(S,beta);
			real SB2 = SB*SB;

			for(j=max(0,i-n2);j<=min(i+n2,(long)V.depth-1);j++)
			{
				real aj = V.get(x,y,j);
				real g = -aj*beta*pow(S,beta-1)*alpha/n*2*aj;
				if(j==i) g += SB;
				g /= SB2;
				g *= chain_grad;
				V.add_grad(x,y,j,g);
			}
		}
	}
};//struct LocalResponseNormalizationLayer

/*
var QuadTransformLayer = function(opt) {
	var opt = opt || {};

	// computed
	this.out_sx = opt.in_sx;
	this.out_sy = opt.in_sy;
	// linear terms, and then quadratic terms, of which there are 1/2*n*(n+1),
	// (offdiagonals and the diagonal total) and arithmetic series.
	// Actually never mind, lets not be fancy here yet and just include
	// terms x_ix_j and x_jx_i twice. Half as efficient but much less
	// headache.
	this.out_depth = opt.in_depth + opt.in_depth * opt.in_depth;
	this.layer_type = 'quadtransform';
}
*/

struct QuadTransformLayer : public Layer
{
	static void init(LayerOpts& _opt)
	{
	}

	QuadTransformLayer(const LayerOpts& _opt)
		:Layer(_opt)
	{
		initialize(_opt);
	}

	virtual void initialize(const LayerOpts& _opt)
	{
		out_sx = _opt.in_sx;
		out_sy = _opt.in_sy;
		out_depth = _opt.in_depth + _opt.in_depth*_opt.in_depth;
		layer_type = layertype_quadtransform;
		out_act.alloc(out_sx,out_sy,out_depth,false,(real)0.0);
	}

	virtual Layer* clone() const
	{
		return trace_alloc(new QuadTransformLayer(*this));
	}

	QuadTransformLayer(const QuadTransformLayer& _qtl)
		:Layer(_qtl)
	{
		out_act.alloc_copy(_qtl.out_act);
	}

	
	/*QuadTransformLayer.prototype = {
	forward: function(V, is_training)
	{
		this.in_act = V;
		var N = this.out_depth;
		var Ni = V.depth;
		var V2 = new Vol(this.out_sx, this.out_sy, this.out_depth, 0.0);
		for(var x=0;x<V.sx;x++) {
			for(var y=0;y<V.sy;y++) {
				for(var i=0;i<N;i++) {
					if(i<Ni) {
						V2.set(x,y,i,V.get(x,y,i)); // copy these over (linear terms)
					} else {
						var i0 = Math.floor((i-Ni)/Ni);
						var i1 = (i-Ni) - i0*Ni;
						V2.set(x,y,i,V.get(x,y,i0) * V.get(x,y,i1)); // quadratic
					}
				}
			}
		}
		this.out_act = V2;
		return this.out_act; // dummy identity function for now
	},*/

	virtual void forward(bool _is_training)
	{
		Vol& V = *in_act;
		size_t N = out_depth;
		size_t Ni = V.depth;
		Vol& V2 = out_act;
		size_t x=0,y=0,i=0;
		for(y=0;y<V.sy;y++)
		{
			for(x=0;x<V.sx;x++)
			{
				for(i=0;i<N;i++)
				{
					if(i<Ni)
					{
						V2.set(x,y,i,V.get(x,y,i));
					}
					else
					{
						size_t i0 = (i - Ni)/Ni;
						size_t i1 = (i - Ni)%Ni;
						V2.set(x,y,i,V.get(x,y,i0)*V.get(x,y,i1));
					}
				}
			}
		}
	}
	
	/*backward: function()
	{
		var V = this.in_act;
		V.dw = global.zeros(V.w.length); // zero out gradient wrt data
		var V2 = this.out_act;
		var N = this.out_depth;
		var Ni = V.depth;
		for(var x=0;x<V.sx;x++) {
			for(var y=0;y<V.sy;y++) {
				for(var i=0;i<N;i++) {
					var chain_grad = V2.get_grad(x,y,i);
					if(i<Ni) {
						V.add_grad(x,y,i,chain_grad);
					} else {
						var i0 = Math.floor((i-Ni)/Ni);
						var i1 = (i-Ni) - i0*Ni;
						V.add_grad(x,y,i0,V.get(x,y,i1)*chain_grad);
						V.add_grad(x,y,i1,V.get(x,y,i0)*chain_grad);
					}
				}
			}
		}
	},*/
	virtual void backward(bool _blearn)
	{
		Vol& V = *in_act;
		V.zero_dw();
		Vol& V2 = out_act;
		size_t N = out_depth;
		size_t Ni = V.depth;
		size_t x=0,y=0,i=0;
		for(y=0;y<V.sy;y++)
		{
			for(x=0;x<V.sx;x++)
			{
				for(i=0;i<N;i++)
				{
					real chain_grad = V2.get_grad(x,y,i);
					if(i<Ni)
					{
						V.add_grad(x,y,i,chain_grad);
					}
					else
					{
						size_t i0 = (i-Ni)/Ni;
						size_t i1 = (i-Ni)%Ni;
						V.add_grad(x,y,i0,V.get(x,y,i1)*chain_grad);
						V.add_grad(x,y,i1,V.get(x,y,i0)*chain_grad);
					}
				}
			}
		}
	}

};//struct QuadTransformLayer


typedef std::list<LayerOpts> LayerOptsLst;

struct NetBase
{
	virtual Vol& get_input() = 0;
	virtual Vol& get_target() = 0;
	virtual Vol& get_result() = 0;
	virtual void forward(bool _is_training = false)= 0;
	virtual real test() = 0;
	virtual real backward() = 0;
	virtual void getParamsAndGrads(ParamsAndGradsVec& _response) const = 0;
};

struct LayerFactory
{
	Layer* create(LayerOpts& _opt)
	{
		if(NOT_NULL(_opt.original))
			return get_original(_opt);

		return create_new_object(_opt);
	}

protected:
	Layer* get_original(LayerOpts& _opt)
	{
		VERIFY_EXIT1(NOT_NULL(_opt.original),NULL);
		_opt.original->initialize(_opt);
		return _opt.original;
	}

	Layer* create_clonable(LayerOpts& _opt)
	{
		VERIFY_EXIT1(NOT_NULL(_opt.original),NULL);
		Layer* new_layer = _opt.original->clone();
		new_layer->initialize(_opt);
		return new_layer;
	}

	Layer* create_new_object(LayerOpts& _opt)
	{
		switch(_opt.type)
		{
		case layertype_input: 
			return trace_alloc(new InputLayer(_opt));
		case layertype_fc:
			return trace_alloc(new FullyConnLayer(_opt));
		case layertype_relu:
			return trace_alloc(new ReluLayer(_opt));
		case layertype_sigmoid:
			return trace_alloc(new SigmoidLayer(_opt));
		case layertype_regression:
			return trace_alloc(new RegressionLayer(_opt));
		case layertype_conv:
			return trace_alloc(new ConvLayer(_opt));
		case layertype_pool:
			return trace_alloc(new PoolLayer(_opt));
		case layertype_softmax:
			return trace_alloc(new SoftmaxLayer(_opt));
		case layertype_maxout:
			return trace_alloc(new MaxoutLayer(_opt));
		case layertype_dropout:
			return trace_alloc(new DropoutLayer(_opt));
		case layertype_lrn:
			return trace_alloc(new LocalResponseNormalizationLayer(_opt));
		case layertype_quadtransform:
			return trace_alloc(new QuadTransformLayer(_opt));
		case layertype_tanh:
			return trace_alloc(new TanhLayer(_opt));
		case layertype_hidden_input:
			return trace_alloc(new HiddenInput(_opt));
		case layertype_hidden_regression:
			return trace_alloc(new HiddenRegression(_opt));
		}
		VERIFY_EXIT1(FALSE,NULL);
		return NULL;
	}
};//struct LayerFactory

struct Net : public NetBase
{
protected:
	struct LayerInitilizer
	{
		Vol* input_vol;

		LayerInitilizer()
			:input_vol(NULL)
		{
		}

		void operator()(Layer* _layer)
		{
			if(_layer->layer_type==layertype_input)
			{
				input_vol = &(_layer->out_act);
			}
			else
			{
				VERIFY_EXIT(NOT_NULL(input_vol));
				_layer->set_input(input_vol);
			}
		}
	};//struct LayerInitilizer

public:
	ptrvector<Layer> m_layers;
	Vol m_target;

protected:
	LayerFactory m_factory;

public:
	Net()
	{
	}

	~Net()
	{
		free();	
	}

	void free()
	{
		m_layers.clear();
	}

	Net(const Net& _net)
	{
		if(!_net.m_layers.empty())
		{
			LayerInitilizer layer_initializer;
			m_layers.resize(_net.m_layers.size());
			size_t i=0;
			for(i=0;i<m_layers.size();i++)
			{
				m_layers[i] = _net.m_layers[i]->clone();
				layer_initializer(m_layers[i]);
			}
		}
	}

	virtual Vol& get_input()
	{
		VERIFY_EXIT1(!m_layers.empty(),Fish<Vol>::get());
		return m_layers[0]->get_input();
	}

	virtual Vol& get_target()
	{
		return m_target;
	}

	bool any_input_nan() const 
	{
		const Vol& input = const_cast<Net&>(*this).get_input();
		size_t i = 0;
		for(i=0;i<input.w.size();i++)
		{
			if(!(input.w[i]==input.w[i])) // test for nan, (input.w[i]==input.w[i]) == false if input.w[i] is nan
				return true;
		}
		return false;
	}

	bool any_target_nan() const 
	{
		const Vol& input = const_cast<Net&>(*this).get_target();
		size_t i = 0;
		for(i=0;i<input.w.size();i++)
		{
			if(!(input.w[i]==input.w[i])) // test for nan, (input.w[i]==input.w[i]) == false if input.w[i] is nan
				return true;
		}
		return false;
	}

	virtual Vol& get_result()
	{
		VERIFY_EXIT1(!m_layers.empty(),Fish<Vol>::get());
		return m_layers[m_layers.size()-1]->get_result();
	}

	LayerOptsLst desugar(const LayerOptsLst& _defs)
	{
		LayerOptsLst new_defs;
		LayerOptsLst::const_iterator
			it = _defs.begin()
			,ite = _defs.end()
			;
		for(;it!=ite;++it)
		{
			LayerOpts def = *it;
			if(def.type==layertype_softmax)
			{
				LayerOpts opt;
				opt.type = layertype_fc;
				opt.num_neurons = def.num_classes;
				new_defs.push_back(opt);
			}
			if(def.type==layertype_regression)
			{
				LayerOpts opt;
				opt.type = layertype_fc;
				opt.num_neurons = def.num_neurons;
				new_defs.push_back(opt);
			}
			if(def.type==layertype_fc
				&& !def.b_bias_pref)
			{
				def.b_bias_pref = true;
				def.bias_pref = 0;
				if(def.activation==layertype_relu)
				{
					def.bias_pref = (real)0.1;
				}
			}
			if(def.tensor)
			{
				LayerOpts opt;
				opt.type = layertype_quadtransform;
				new_defs.push_back(opt);
			}

			new_defs.push_back(def);

			if(def.activation==layertype_relu)
			{
				LayerOpts opt;
				opt.type = layertype_relu;
				new_defs.push_back(opt);
			}
			else if(def.activation==layertype_sigmoid)
			{
				LayerOpts opt;
				opt.type = layertype_sigmoid;
				new_defs.push_back(opt);
			}
			else if(def.activation==layertype_tanh)
			{
				LayerOpts opt;	
				opt.type = layertype_tanh;
				new_defs.push_back(opt);
			}
			else if(def.activation==layertype_maxout)
			{
				LayerOpts opt;	
				opt.type = layertype_maxout;
				opt.group_size = def.group_size;
				new_defs.push_back(opt);
			}
			else if(def.b_drop_prob && def.type!=layertype_dropout)
			{
				LayerOpts opt;	
				opt.type = layertype_dropout;
				opt.drop_prob = def.drop_prob;
				opt.start_drop = def.start_drop;
				opt.stop_drop = def.stop_drop;
				new_defs.push_back(opt);
			}
		}
		return new_defs;
	}

	void makeLayers(const LayerOptsLst& _defs)
	{
		m_layers.clear();
		LayerOptsLst defs = desugar(_defs);
		LayerOpts prev;
		LayerOptsLst::iterator
			it = defs.begin()
			,ite = defs.end()
			;
		bool bfirst = true;
		LayerInitilizer layer_initializer;
		for(;it!=ite;++it)
		{
			LayerOpts& def = *it;
			if(!bfirst)
			{
				def.in_sx = m_layers[m_layers.size()-1]->out_sx;
				def.in_sy = m_layers[m_layers.size()-1]->out_sy;
				def.in_depth = m_layers[m_layers.size()-1]->out_depth;
			}

			m_layers.push_back(m_factory.create(def));
			layer_initializer(m_layers.back());

			if(!bfirst)
			{
				m_layers[m_layers.size()-1]->in_act = &(m_layers[m_layers.size()-2]->out_act);
			}
			bfirst = false;
		}
		if(!m_layers.empty())
		{
			m_layers[m_layers.size()-1]->start_learning = true;
			m_target.alloc(
				m_layers[m_layers.size()-1]->out_sx
				,m_layers[m_layers.size()-1]->out_sy
				,m_layers[m_layers.size()-1]->out_depth
				,false
				);
		}
	}

	void forward(bool _is_training = false)	
	{
		VERIFY_EXIT(!any_input_nan());
		size_t i=0;
		for(i=0;i<m_layers.size();i++)
		{
			m_layers[i]->forward(_is_training);
		}
	}

	real test()
	{
		VERIFY_EXIT1(!any_input_nan(),(real)0);
		VERIFY_EXIT1(!any_target_nan(),(real)0);
		long N = (long)m_layers.size();
		real loss = m_layers[N-1]->backward(m_target,false);
		return loss;
	}

	real backward()
	{
		VERIFY_EXIT1(!any_input_nan(),(real)0);
		VERIFY_EXIT1(!any_target_nan(),(real)0);
		long N = (long)m_layers.size();
		bool blearn = m_layers[N-1]->start_learning;
		blearn &= !m_layers[N-1]->stop_learning;
		real loss = m_layers[N-1]->backward(m_target,blearn);
		long i=0;
		for(i=N-2;i>=0;i--)
		{
			blearn |= m_layers[i]->start_learning;
			blearn &= !m_layers[i]->stop_learning;
			m_layers[i]->backward(blearn);
		}
		return loss;
	}

	void getParamsAndGrads(ParamsAndGradsVec& _response) const
	{
		_response.clear();
		size_t i=0;
		for(i=0;i<m_layers.size();i++)
		{
			ParamsAndGradsVec layer_response;
			m_layers[i]->getParamsAndGrads(layer_response);
			_response.insert(_response.end(),layer_response.begin(),layer_response.end());
		}
	}

	void clean_train()
	{
		ParamsAndGradsVec pglist;
		pglist.reserve(m_layers.size()*2);
		getParamsAndGrads(pglist);
		size_t i=0;
		for(i=0;i<pglist.size();i++)
		{
			ParamsAndGrads& pg = pglist[i];
			Vol::WT& p = *pg.params;	// w
			Vol::WT& g = *pg.grads;		// dw
			std::fill(g.begin(),g.end(),(real)0.0);
		}
	}

	void copy_from(const Net& _net)
	{
		ParamsAndGradsVec layer_response_dest;
		getParamsAndGrads(layer_response_dest);
		ParamsAndGradsVec layer_response_src;
		_net.getParamsAndGrads(layer_response_src);

		VERIFY_EXIT(layer_response_dest.size()==layer_response_src.size());

		ParamsAndGradsVec::const_iterator
			pit_dest = layer_response_dest.begin()
			,pite_dest = layer_response_dest.end()
			;
		ParamsAndGradsVec::const_iterator
			pit_src = layer_response_src.begin()
			,pite_src = layer_response_src.end()
			;
		for(;pit_dest!=pite_dest && pit_src!=pite_src;++pit_dest,++pit_src)
		{
			const ParamsAndGrads& pdest = *pit_dest;
			const ParamsAndGrads& psrc = *pit_src;
			VERIFY_DO(NOT_NULL(pdest.params),continue);
			VERIFY_DO(pdest.params->size()==psrc.params->size(),continue);
			std::copy(psrc.params->begin(),psrc.params->end(),pdest.params->begin());
		}
	}

	template<typename _Stream>
	void save(_Stream& _out) const
	{
		size_t i=0;
		for(i=0;i<m_layers.size();i++)
		{
			ParamsAndGradsVec layer_response;
			m_layers[i]->getParamsAndGrads(layer_response);
			ParamsAndGradsVec::const_iterator
				pit = layer_response.begin()
				,pite = layer_response.end()
				;
			for(;pit!=pite;++pit)
			{
				const ParamsAndGrads& p = *pit;
				VERIFY_DO(NOT_NULL(p.params),continue);
				size_t i=0;
				for(i=0;i<p.params->size();i++)
				{
					CString s;
					s.Format(_T("%1.18g\t"),(*p.params)[i]);
					_out << (LPCTSTR)s;
				}
				_out << _T("\n");
			}
		}
	}


	void binary_save(const CString& _file_name) const
	{
		BinFile<real> output;
		output.open_to_write(_file_name);

		ParamsAndGradsVec layer_response;
		getParamsAndGrads(layer_response);

		ParamsAndGradsVec::const_iterator
			pit = layer_response.begin()
			,pite = layer_response.end()
			;
		for(;pit!=pite;++pit)
		{
			const ParamsAndGrads& p = *pit;
			VERIFY_DO(NOT_NULL(p.params),continue);
			output.put(p.params->begin(),p.params->end());
		}
	}

	template<typename _Stream>
	void load(_Stream& _in)
	{
		size_t i=0;
		for(i=0;i<m_layers.size();i++)
		{
			ParamsAndGradsVec layer_response;
			m_layers[i]->getParamsAndGrads(layer_response);
			ParamsAndGradsVec::const_iterator
				pit = layer_response.begin()
				,pite = layer_response.end()
				;
			for(;pit!=pite;++pit)
			{
				const ParamsAndGrads& p = *pit;
				VERIFY_DO(NOT_NULL(p.params),continue);
				size_t j=0;
				for(j=0;j<p.params->size();j++)
				{
					_in >> (*p.params)[j]; 
					//_in >> _T("\t");
				}
				//_in >> _T("\n");
				VERIFY_DO(NOT_NULL(p.grads),continue);
				for(j=0;j<p.grads->size();j++)
				{
					(*p.grads)[j] = 0;
				}
			}
		}
	}

	void binary_load(const CString& _file_name)
	{
		BinFile<real> input;
		input.open_to_read(_file_name);

		ParamsAndGradsVec layer_response;
		getParamsAndGrads(layer_response);

		ParamsAndGradsVec::const_iterator
			pit = layer_response.begin()
			,pite = layer_response.end()
			;
		for(;pit!=pite;++pit)
		{
			const ParamsAndGrads& p = *pit;
			VERIFY_DO(NOT_NULL(p.params),continue);
			size_t cnt = input.get(p.params->begin(),p.params->end());
			VERIFY_DO(NOT_NULL(p.grads),continue);
			size_t i = 0;
			for(i=0;i<p.grads->size();i++)
			{
				(*p.grads)[i] = 0;
			}
		}
	}

	void save_file(const CString& _file_name)
	{
		std::tofstream out;
		out.open(_file_name);
		save(out);
	}

	void load_file(const CString& _file_name)
	{
		if(!FileUtils::IsFileExist(_file_name)) return;
		std::tifstream inp;
		inp.open(_file_name);
		load(inp);
	}
};

struct TrainerOptions
{
	real learning_rate;
	real l1_decay;
	real l2_decay;
	size_t batch_size;
	real momentum;

	TrainerOptions()
		:learning_rate((real)0.01)
		,l1_decay(0)
		,l2_decay(0)
		,batch_size(1)
		,momentum((real)0.9)
	{
	}
};

struct SGDTrainer : public TrainerOptions
{
	Net& net;
	size_t k;
	std::vector<Vol::WT> last_pg;

	SGDTrainer(Net& _net,const TrainerOptions& _options = TrainerOptions())
		:TrainerOptions(_options)
		,net(_net)
		,k(0)
	{
	}

	void set_options(const TrainerOptions& _options)
	{
		static_cast<TrainerOptions&>(*this) = _options;
	}

	Vol& get_input() {return net.get_input();}
	Vol& get_target() {return net.get_target();}

	void test_loss(real& _cost_loss)
	{
		net.forward(true);
		_cost_loss = net.test();
	}

	void train(
		DWORD& _fwd_time,DWORD& _bwd_time
		,real& _l2_decay_loss
		,real& _l1_decay_loss
		,real& _cost_loss
		,real& _loss
		//,real _k = 1.0
		)
	{
		DWORD start = GetTickCount();
		net.forward(true);
		DWORD end = GetTickCount();
		_fwd_time = end-start;

		start = GetTickCount();
		_cost_loss = net.backward();
		_l2_decay_loss = 0.0;
		_l1_decay_loss = 0.0;
		end = GetTickCount();
		_bwd_time = end-start;
		batch(_l2_decay_loss,_l1_decay_loss,_cost_loss,_loss);
	}

	void batch(
		real& _l2_decay_loss
		,real& _l1_decay_loss
		,real& _cost_loss
		,real& _loss
		)
	{
		if(k && k%batch_size==0)
		{
			ParamsAndGradsVec pglist;
			pglist.reserve(net.m_layers.size()*2);
			net.getParamsAndGrads(pglist);
			if(last_pg.size()==0 && momentum>0)
			{
				size_t i=0;
				for(i=0;i<pglist.size();i++)
				{
					last_pg.push_back(Vol::WT());
					last_pg.back().resize(pglist[i].params->size());
				}
			}

			size_t i=0;
			for(i=0;i<pglist.size();i++)
			{
				ParamsAndGrads& pg = pglist[i];
				Vol::WT& p = *pg.params;	// w
				Vol::WT& g = *pg.grads;		// dw

				real l2_decay_mul  = pg.l2_decay_mul;
				real l1_decay_mul = pg.l1_decay_mul;
				real l2_decay1 = l2_decay*l2_decay_mul;
				real l1_decay1 = l1_decay*l1_decay_mul;

				size_t plen = p.size();
				size_t j=0;
				for(j=0;j<plen;j++)
				{
					_l2_decay_loss += l2_decay1*p[j]*p[j]/2;
					_l1_decay_loss += l1_decay1*fabs(p[j]);
					real l1grad = l1_decay1 * (p[j]>0?1:-1);
					real l2grad = l2_decay1 * p[j];
					if(momentum>0)
					{
						real dir = -learning_rate*(l2grad + l1grad + g[j]) / batch_size;
						real dir_adj = momentum * last_pg[i][j] + ((real)1.0 - momentum)*dir;
						p[j] += dir_adj;
						last_pg[i][j] = dir_adj;
					}
					else
					{
						p[j] -= learning_rate * (l2grad + l1grad + g[j]) / batch_size;
					}
					g[j] = 0;
				}
			}
		}
		k++;
		_loss = _l2_decay_loss + _l1_decay_loss + _cost_loss;
	}
};

inline
void init_layer_options(LayerTypeEn _layer_type,LayerOpts& _def)
{
	switch(_layer_type)
	{
	case layertype_input:			InputLayer::init(_def); break;
	case layertype_fc:				FullyConnLayer::init(_def); break;
	case layertype_relu:			ReluLayer::init(_def); break;
	case layertype_sigmoid:			SigmoidLayer::init(_def); break;
	case layertype_regression:		RegressionLayer::init(_def); break;
	case layertype_conv:			ConvLayer::init(_def); break;
	case layertype_pool:			PoolLayer::init(_def); break;
	case layertype_softmax:			SoftmaxLayer::init(_def); break;
	case layertype_maxout:			MaxoutLayer::init(_def); break;
	case layertype_dropout:			DropoutLayer::init(_def); break;
	case layertype_lrn:				LocalResponseNormalizationLayer::init(_def); break;
	case layertype_quadtransform:	QuadTransformLayer::init(_def); break;
	case layertype_tanh:			TanhLayer::init(_def); break;
	default:
		VERIFY_EXIT(false); // layer not initialized
	}
}

namespace common
{
	inline void convert(LayerTypeEn _layer_type,CString& _str)
	{
		switch(_layer_type)
		{
		case layertype_null: _str = _T("layertype_null"); break;
		case layertype_input: _str = _T("layertype_input"); break;
		case layertype_fc: _str = _T("layertype_fc"); break;
		case layertype_relu: _str = _T("layertype_relu"); break;
		case layertype_sigmoid: _str = _T("layertype_sigmoid"); break;
		case layertype_regression: _str = _T("layertype_regression"); break;
		case layertype_conv: _str = _T("layertype_conv"); break;
		case layertype_pool: _str = _T("layertype_pool"); break;
		case layertype_softmax: _str = _T("layertype_softmax"); break;
		case layertype_maxout: _str = _T("layertype_maxout"); break;
		case layertype_dropout: _str = _T("layertype_dropout"); break;
		case layertype_lrn: _str = _T("layertype_lrn"); break;
		case layertype_quadtransform: _str = _T("layertype_quadtransform"); break;
		case layertype_tanh: _str = _T("layertype_tanh"); break;
		case layertype_hidden_input: _str = _T("layertype_hidden_input"); break;
		case layertype_hidden_regression: _str = _T("layertype_hidden_regression"); break;
		case layertype_functional: _str = _T("layertype_functional"); break;
		default:
			 _str = Format(_T("Unknown type (%d)"),(long)_layer_type); 
			 break;
		};

	}
}

#endif//#if !defined(__DLNET_H__259ADFCB_316B_4832_B14E_2928FCCBC10B__)