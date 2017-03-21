struct page_fn
{
  template <typename T, typename Component, typename Path>
  constexpr auto operator()(Component component, Path path)
  {
    return match(path
    , when<nbdl::unresolved>(div(text_content("Loading..."_s)))
    , when<nbdl::not_found>(div(text_content("Not Found!"_s)))
    , when<T>(component)
    , when<>(div(text_content("Something went horribly wrong!"_s)))
    );
  }
};

template <typename T>
constexpr page = hana::curry<2>(page_fn<T>{});

auto campaign = [](auto path_to_campaign)
{
  return div(
    div(
      attr_class("title"_s)
    , text_content(path(path_to_campaign, "title"_s))
    )
  , div(
      attr_class("description"_s)
    , text_content(path(path_to_campaign, "description"_s))
    )
  );
};

auto default_page = div(
  attr_class("default-page")
, text_content("Welcome to the default page."_s)
);

auto main_page = match(nbdl::path<"nav_location"_s>
, when<campaign_page_path>(page<my_app::campaign>(campaign))
, when<>(default_page)
);
