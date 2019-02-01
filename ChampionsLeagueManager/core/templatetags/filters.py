from django import template

register = template.Library()


@register.filter
def iterate_in_range(s):
    split = s.split(",")
    start = int(split[0])
    end = int(split[1])

    return range(start, end)


@register.filter
def index(List, i):
    return List[int(i)]


@register.filter
def absolute_url(List, i):
    return List[int(i)].get_absolute_url()
