/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csg.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: racousin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/09 10:13:40 by racousin          #+#    #+#             */
/*   Updated: 2017/03/09 12:48:43 by racousin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <libft.h>
#include "renderer.h"

/*
** particulary case solve quickly in intersecion interval
**
*/

int	inter_case(t_interval *left, t_interval *right, t_interval *interval)
{
	if (left->nb_hit == 0 || right->nb_hit == 0)
	{
		interval->nb_hit = 0;
		return (0);
	}
	return (1);
}
/*
**
**return 1 if 2 intervals are disjoint, 0 else
*/

int	is_disjoint(t_interval *a_i, t_interval *a_j, int i, int j)
{
	return (a_i->max[i].dist < a_j->min[j].dist || a_i->min[i].dist > a_j->max[j].dist);
}
/*
**result in a_i : intersectin between interval *a_i and *a_j
**
*/

void	modify_by_inter(t_interval *a_i, t_interval *a_j, int i, int j)
{
	if (a_i->min[i].dist > a_j->min[j].dist)
	{
		a_i->min[i] = a_j->min[j];
		if (a_i->max[i].dist < a_j->max[j].dist)
			a_i->max[i] = a_j->max[j];
	}
}
/*
**result in interval : intersectin between all interval in right and left
**
*/

void	inter(t_interval *left, t_interval *right, t_interval *interval)
{
	int	l;
	int	r;
	int	i;

	if (!inter_case(left, right, interval))	
		return;
	l = 0;
	i = 0;
	while (l < left->nb_hit)
	{
		interval->min[i] = left->min[l];
		interval->max[i] = left->max[l];
		r = 0;
		while (r < right->nb_hit)
		{
			if (is_disjoint(interval, right, i, r))
				break;
			else
				modify_by_inter(interval, right, i, r);
			r++;
		}
		if (r == right->nb_hit)
			i++;
		l++;
	}
	interval->nb_hit = i;
}

/*
** particulary case solve quickly in intersecion interval
**
*/

int	minus_case(t_interval *left, t_interval *right, t_interval *interval)
{
	if (left->nb_hit == 0)
	{
		interval->nb_hit = 0;
		return (0);
	}
	else if (right->nb_hit == 0)
	{
		interval = left;
		return (0);
	}
	return (1);
}
/*
**
**test kind intersection : partiel overlap
*/
int	minus_test1(t_interval *a_i, t_interval *a_j, int i, int j)
{
	double	min1;
	double	min2;
	double	max1;
	double	max2;
	
	min1 = a_i->min[i].dist;
	max1 = a_i->max[i].dist;
	min2 = a_j->min[j].dist;
	max2 = a_j->min[j].dist;
	if (min1 < min2 && max1 < min2)
		return (0);
	if (min2 < min1 && max2 < min1)
		return (0);
	return (1);
}
/*
**
**test kind intersection : total overlap
*/
int	minus_test0(t_interval *a_i, t_interval *a_j, int i, int j)
{
	double	min1;
	double	min2;
	double	max1;
	double	max2;
	
	min1 = a_i->min[i].dist;
	max1 = a_i->max[i].dist;
	min2 = a_j->min[j].dist;
	max2 = a_j->min[j].dist;
	if (min1 > min2 && max1 < max2)
		return (0);
	return (1);
}
/*
**
** modify : creat new segment
*/
void	modify_by_minus2(t_interval *a_i, t_interval *a_j, int i, int *j)
{
	a_i->min[i + 1] = a_j->min[*j];
	a_i->max[i + 1] = a_i->max[i];
	a_i->max[i] = a_i->min[*j];
	(*j)++;
}
void	modify_by_minus3(t_interval *a_i, t_interval *a_j, int i)
{
	a_j->min[a_j->nb_hit] = a_i->min[i];
	a_j->max[a_j->nb_hit] = a_i->max[i];
	(a_j->nb_hit)++;
}

/*
**
**modify : eat a_j in a_i
*/
void	modify_by_minus1(t_interval *a_i, t_interval *a_j, int i, int *j)
{
	if (a_i->min[i].dist < a_j->min[*j].dist)
		a_i->max[i] = a_j->min[*j];
	else
		a_i->min[i] = a_j->max[*j];
	(*j)++;
}

/*
**
**result in interval : left minus right
*/

void	minus(t_interval *left, t_interval *right, t_interval *interval)
{
	int	l;
	int	r;
	int	i;

	if (!minus_case(left, right, interval))	
		return;
	l = 0;
	i = 0;
	while (l < left->nb_hit)
	{
		interval->min[i] = left->min[l];
		interval->max[i] = left->max[l];
		r = 0;
		while (r < right->nb_hit)
		{
			if (is_disjoint(interval, right, i, r))
			{
				r++;
				continue;
			}
			if (minus_test1(interval, right, i, r))
				modify_by_minus1(interval, right, i, &r);
			else if (minus_test0(interval, right, i, r))
				break;
			else
			{
				modify_by_minus2(interval, right, i, &r);
				modify_by_minus3(interval, left, i + 1);
			}
		}
		if  (r == right->nb_hit)
			i++;
		l++;
	}
	interval->nb_hit = i;
}

/*
**recursion fonction which run csg tree_obj, and return in intervall, segments cross by ray
**and the ref of the normal for each of them
*/

void			test_csg(t_obj *obj, t_ray *ray, t_interval *interval)
{
	t_interval	left;
	t_interval	right;

	if (obj->csg == '0')
		//TODO faire une generique dist_obj(obj, ray) qui remplit interval;
		//pour l'instant uniquement cas sphere :
		{
			double		b;
			double		c;
			t_vec3		ray_sphere;

			ray_sphere = vec3_sub(ray->pos, obj->pos);
			b = vec3_dot(ray_sphere, ray->dir);
			c = vec3_norm2(ray_sphere) - obj->param;
			if((interval->nb_hit = norm_quad_solve2(b, c, interval)))
			{
				interval->min[0].ref_normal = obj;
				interval->max[0].ref_normal = obj;
			}
			
		}
	else
	{
		test_csg(obj->left, ray, &left);
		test_csg(obj->right, ray, &right);
		if (obj->csg == 'U')
			inter(&left, &right, interval);
		else if (obj->csg == 'I')
			inter(&left, &right, interval);
		else if (obj->csg == '-')
			minus(&left, &right, interval);
	}
}

/*
** fill distance minimal positive and ref_normal_obj
** @return 0 if no distance 1 else
** 
*/

int	minimal_positiv(t_interval *interval, t_obj *obj, double *d)
{
	int	i;

	if (interval->nb_hit == 0)
		return (0);
	i = 1;
	*d = interval->min[0].dist; 
	obj->csg_normal_ref = interval->min[0].ref_normal;
	obj->csg_normal = interval->min[0].normal;
	while (i < interval->nb_hit)
	{
		if (*d > interval->min[i].dist)
		{
			*d = interval->min[i].dist;
			obj->csg_normal_ref = interval->min[i].ref_normal;
		}
		i++;
	}
	return (1);
}
/*
** Intersection between ray and csg_obj
** @return the distance between camera and object if there is collision
** or a negative value otherwise
*/

int				csg_intersect(t_obj *self, t_ray *ray)
{
	t_interval	interval;
	double	d;
	//TODO A REMPLIR DYNAMIQUEMENT
	/*t_obj	left;
//	t_obj	left_left;
//	t_obj	left_right;
	t_obj	right;
	left = *self;
	right = *self;
	left.pos.x=-1;
	right.pos.x=0;
	self->csg = 'I';
	self->left = &left;
	self->right = &right;
	self->left->csg = '0';
//	left_right = left;
//	left_left = left;
//	left_left.pos.x=0.5;
//	self->left->left = &left_left;
//	self->left->right = &left_right;
//	self->left->left->param = 1;
//	self->left->right->param = 1;
//	self->left->left->csg = '0';
//	self->left->right->csg = '0';
	self->right->csg = '0';
	self->right->param = 1;
	self->left->param = 1;
	//
	//*/
	test_csg(self, ray, &interval);
	if (minimal_positiv(&interval, self, &d))
	{
		ray->t = d;
		if (ray->type == INITIAL_RAY)
			ray->collided = self;
		return (1);
	}
	return (0);
}

/*
** Normal vector at given point
** @return normal direction
*/

t_vec3			csg_normal(t_obj *self, t_vec3 pos)
{
	t_obj *obj;

	obj = self->csg_normal_ref;
	if(!self->csg_normal)
	{
	return (vec3_get_normalized(vec3_sub(pos, obj->pos)));
	}
	else
	{
		printf("test2");
	return (vec3_get_normalized(vec3_sub(obj->pos, pos)));
	}
}
