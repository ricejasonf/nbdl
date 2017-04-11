struct page_fn
{
  template <typename T, typename Component, typename Path>
  constexpr auto operator()(Component component, Path path_)
  {
    return div(
      attr_class("page"_s)
    , root_match(path_
      , when<nbdl::unresolved>(div(text_node("Loading..."_s)))
      , when<nbdl::not_found>(div(text_node("Not Found!"_s)))
      , when<T>(component)
      , otherwise(div(text_node("Something went horribly wrong!"_s)))
      )
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
    , text_node(get(path_to_campaign, "title"_s))
    )
  , div(
      attr_class("description"_s)
    , text_node(get(path_to_campaign, "description"_s))
    )
  , div(
      attr_class("note"_s)
    , text_area(
        match(path(path_to_campaign, "note"_s) // optional
        , when<my_app::note>(text_node("pending"_s))
        , otherwise()
        )
      )
    ) 
  , div(
      attr_class("status"_s)
    , match(path(path_to_campaign, "status"_s)
      , when<campaign_status::pending>(text_node("pending"_s))
      , when<campaign_status::active>(text_node("active"_s))
      , when<campaign_status::finished>(text_node("finished"_s))
      , otherwise()
      )
    )
  );
};

auto default_page = div(
  attr_class("default-page")
, text_node("Welcome to the default page."_s)
);

auto main_page = match(/* root_store, */ path("nav_location"_s)
, when<campaign_page_path>(page<my_app::campaign>(campaign))
, when<>(default_page)
);

/*
 * match inputs: path_spec, when...
 */
