from django.shortcuts import render
from .models import *
from django.db.models import Q
from itertools import chain
import operator


def build_calendar_entries(matches):
    entries = []
    for match in matches:
        home_club_footballers = match.referred_match.home_club.footballers.all()
        home_score = Goal.objects.filter(match=match.referred_match, scorer__in=home_club_footballers).count()
        away_club_footballers = match.referred_match.away_club.footballers.all()
        away_score = Goal.objects.filter(match=match.referred_match, scorer__in=away_club_footballers).count()
        entries.append({"match_object": match, "home_score": home_score, "away_score": away_score})

    return entries


def homepage(request):
    return render(request=request, template_name="homepage.html")


def calendar(request):
    context = {"groups_calendar": [], "knockouts_calendar": []}

    for i in range(1, 6 + 1):
        day_matches = GroupMatch.objects.filter(day=i).order_by("group")
        to_add = {"day": i, "matches": build_calendar_entries(day_matches)}
        context["groups_calendar"].append(to_add)

    for turn in range(1, 4 + 1):
        turn_matches = KnockoutMatch.objects.filter(turn=turn).order_by("referred_match__date")
        to_add = {"turn": turn, "matches": build_calendar_entries(turn_matches)}
        context["knockouts_calendar"].append(to_add)

    return render(request=request, template_name="calendar.html", context=context)


def clubs(request):
    context = {
        "clubs": list(Club.objects.all().order_by("name"))
    }
    return render(request=request, template_name="clubs.html", context=context)


def club_details(request, pk):
    club = Club.objects.get(pk=pk)
    context = {"club": club, "home_matches": [], "away_matches": []}

    for match in club.home_matches.all().order_by("date"):
        away_club = match.away_club
        club_score = Goal.objects.filter(match=match.pk, scorer__in=club.footballers.all()).count()
        other_score = Goal.objects.filter(match=match.pk, scorer__in=away_club.footballers.all()).count()
        context["home_matches"].append({"away_club": away_club, "club_score": club_score, "other_score": other_score,
                                        "match_object": match})

    for match in club.away_matches.all().order_by("date"):
        home_club = match.home_club
        club_score = Goal.objects.filter(match=match.pk, scorer__in=club.footballers.all()).count()
        other_score = Goal.objects.filter(match=match.pk, scorer__in=home_club.footballers.all()).count()
        context["away_matches"].append({"home_club": home_club, "club_score": club_score, "other_score": other_score,
                                        "match_object": match})

    return render(request=request, template_name="club_details.html", context=context)


def groups(request):
    context = {"groups": []}

    for group in Group.objects.all():
        letter = group.letter
        to_add = {"letter": letter, "clubs_and_stats": []}
        for club in Club.objects.filter(group=letter).all():
            club_footballers = Footballer.objects.filter(club=club)
            club_matches = Match.objects.filter(Q(home_club=club) | Q(away_club=club), is_group_match__isnull=False)
            matches_count = GroupMatch.objects.filter(referred_match__in=club_matches).count()
            goals_scored = Goal.objects.filter(match__in=club_matches, scorer__in=club.footballers.all()).count()
            goals_conceded = Goal.objects.filter(match__in=club_matches).exclude(
                scorer__in=club.footballers.all()).count()
            points, wins, draws, losses = 0, 0, 0, 0
            for match in club_matches:
                scored = Goal.objects.filter(match=match, scorer__in=club_footballers).count()
                conceded = Goal.objects.filter(match=match).exclude(scorer__in=club_footballers).count()
                if scored > conceded:
                    points += 3
                    wins += 1
                elif scored == conceded:
                    points += 1
                    draws += 1
                else:
                    losses += 1

            club_entry_to_add = {"club": club, "matches_count": matches_count, "goals_scored": goals_scored,
                                 "goals_conceded": goals_conceded, "points": points, "wins": wins, "draws": draws,
                                 "losses": losses}
            to_add["clubs_and_stats"].append(club_entry_to_add)
            to_add["clubs_and_stats"] = sorted(to_add["clubs_and_stats"], key=operator.itemgetter('points'),
                                               reverse=True)
        context["groups"].append(to_add)

    return render(request=request, template_name="groups.html", context=context)


def build_club_entry(club, match):
    goals = Goal.objects.filter(match=match, scorer__in=club.footballers.all()).order_by("minute")
    yellow_cards = YellowCard.objects.filter(match=match, admonished__in=club.footballers.all())
    red_cards = RedCard.objects.filter(match=match, expelled__in=club.footballers.all())
    cards = sorted(chain(yellow_cards, red_cards), key=lambda card: card.minute, reverse=False)
    starting_xi = Presence.objects.filter(match=match, footballer__in=club.footballers.all(), is_in_starting_XI=True)
    subs = Substitution.objects.filter(match=match, replaced__in=starting_xi.values("footballer"))
    bench = Presence.objects.filter(match=match, footballer__in=club.footballers.all(), is_in_starting_XI=False)

    entry = {"score": goals.count(), "goals": goals, "cards": cards, "subs": subs, "starting_XI": starting_xi,
             "bench": bench}
    return entry


def match_details(request, pk):
    context = {"match": None,
               "home_club": {"score": 0, "goals": [], "cards": [], "subs": [], "starting_XI": [], "bench": []},
               "away_club": {"score": 0, "goals": [], "cards": [], "subs": [], "starting_XI": [], "bench": []}
               }

    match = Match.objects.get(pk=pk)
    home_club = Club.objects.get(name=match.home_club)
    away_club = Club.objects.get(name=match.away_club)
    context["match"] = match

    context["home_club"] = build_club_entry(home_club, match)
    context["away_club"] = build_club_entry(away_club, match)

    return render(request=request, template_name="match_details.html", context=context)


def footballer_details(request, pk):
    context = {"footballer": Footballer.objects.get(pk=pk)}
    return render(request=request, template_name="footballer_details.html", context=context)
