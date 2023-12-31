#include "spider.h"

const char	*find_domain_name(const char *url)
{
	char	*ptr;

	ptr = strstr(url, "//");
	if (ptr)
	{
		ptr++;
		if (ptr)
			ptr++;
		ptr = strstr((const char *)ptr, "/");
		if (ptr)
			*ptr = 0;
	}
	return (url);
}

int	build_url(const char *img_url, const char *url)
{
	char	*dest;

	if (strncmp("https://", img_url, 8) == 0)
	{
		dest = (char *)img_url;
		handle_img_url(dest);	
		return (0);
	}
	else
	{
		if (strncmp("/", img_url, 1) == 0)
		{
			dest = (char*)calloc(1, strlen(img_url) + strlen(url) + 1);
			if (!dest)
				return (1);
			if (dest)
			{
				strncpy(dest, find_domain_name(url), strlen(url));
				strcat(dest, img_url);
				printf("%s\n", dest);
			}
		}
		else
		{	
			dest = (char*)calloc(1, strlen(img_url) + strlen(url) + 2);
			if (!dest)
				return (1);
			if (dest)
			{
				strncpy(dest, find_domain_name(url), strlen(url));
				strcat(dest, "/");
				strcat(dest, img_url);
				printf("%s\n", dest);
			}
		}
	}
	handle_img_url(dest);	
	free(dest);
	return (0);
}

int	get_img_attr(TidyNode child, const char *url)
{
	TidyAttr	attr;

	attr = tidyAttrFirst(child);
	while (attr)
	{
		if (strstr(tidyAttrName(attr), "src"))
		{
			printf("attr_name:%s\nurlimg:%s\n", tidyAttrName(attr), tidyAttrValue(attr));
			if (build_url(tidyAttrValue(attr), url))
				return (1);
			break;
		}
		attr = tidyAttrNext(attr);
	}
	return (0);
}

int	html_data_parsing(TidyDoc doc, TidyNode tnod, int indent, int lvl, const char *url)
{
	TidyNode child;
	std::vector<TidyAttr> links;
	std::vector<TidyAttr>::iterator	it;

	(void) lvl;
	child = tidyGetChild(tnod);
	while (child)
	{
		ctmbstr name = tidyNodeGetName(child);
		if (name)	/* if it has a name, then it's an HTML tag ... */
		{	TidyAttr	href;

			href = tidyAttrGetById(child, TidyAttr_HREF);
			if (href)
				links.push_back(href);
			if (strcmp("img", name) == 0)
				get_img_attr(child, url);

		}
 		if (html_data_parsing(doc, child, indent + 4, lvl, url))
			return (1);
		child = tidyGetNext(child);
		//recursive
	}
	it = links.begin();
	while (it != links.end())
	{
		if (new_link(lvl, url, *it))
			return (1);
		it++;
	}
	return (0);
}
