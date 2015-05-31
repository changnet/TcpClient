
bool CProtoc::parse_input(const QString &msg_name, const QString &json)
{
    google::protobuf::Message *pmsg = get_msg( msg_name );
    if ( !pmsg )
    {
        m_str_err = EC_NO_MSG % msg_name ;
        return false;
    }

    const QByteArray &bytes = json.toUtf8();

    QJsonParseError jpe;
    QJsonDocument jd = QJsonDocument::fromJson( bytes,&jpe );
    if ( QJsonParseError::NoError != jpe.error )
    {
        m_str_err = EC_JSON_ERR % jpe.errorString();
        return false;
    }

    if ( !jd.isObject() ) //to convert to pb,must be object
    {
        m_str_err = EC_JS_NOT_OBJ;
        return false;
    }

    bool ret = json_object_to_pb( jd.object(),pmsg );

    qDebug() << pmsg->DebugString().c_str();
    return ret;
}


/**
 * @brief CProtoc::fill_field
 * @param field
 * @param value
 * @return
 * 填充一个字段
 */
bool CProtoc::fill_field(google::protobuf::Message *pmsg,
                         const google::protobuf::FieldDescriptor *field, const QJsonValue &value)
{
    const google::protobuf::Reflection *reflection = pmsg->GetReflection();

    switch( field->cpp_type() )
    {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32 :
        reflection->SetInt32(pmsg,field,value.toInt());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64 :
        reflection->SetInt64(pmsg,field,static_cast<google::protobuf::int64>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32 :
        reflection->SetUInt32(pmsg,field,static_cast<google::protobuf::uint32>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64 :
        reflection->SetUInt64(pmsg,field,static_cast<google::protobuf::uint64>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE :
        reflection->SetDouble(pmsg,field,value.toDouble());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT :
        reflection->SetFloat(pmsg,field,static_cast<float>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL :
        reflection->SetBool(pmsg,field,value.toBool());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM :
        /* If the value does not correspond to a known enum value ...*/
        reflection->SetEnumValue(pmsg,field,value.toInt());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING :
        reflection->SetString(pmsg,field,value.toString().toStdString());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE :
    {
        if ( !value.isObject() ) //嵌套必须对应object
        {
            m_str_err = EC_FIELD_NOT_MATCH % QString(field->full_name().c_str()) % " expect object";
            return false;
        }

        google::protobuf::Message *sub_msg = reflection->MutableMessage( pmsg,field );
        json_object_to_pb( value.toObject(),sub_msg );

    }break;
    default:
        m_str_err = QString("unknow type when fill field:%1").arg(field->full_name().c_str());
        return false;
    }

    return true;
}

/**
 * @brief CProtoc::add_field
 * @param pmsg
 * @param field
 * @param value
 * @return
 * 给数组类型字段添加元素
 */
bool CProtoc::add_field(google::protobuf::Message *pmsg,
                        const google::protobuf::FieldDescriptor *field, const QJsonValue &value)
{
    const google::protobuf::Reflection *reflection = pmsg->GetReflection();
    switch( field->cpp_type() )
    {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32 :
        reflection->AddInt32(pmsg,field,value.toInt());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64 :
        reflection->AddInt64(pmsg,field,static_cast<google::protobuf::int64>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32 :
        reflection->AddUInt32(pmsg,field,static_cast<google::protobuf::uint32>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64 :
        reflection->AddUInt64(pmsg,field,static_cast<google::protobuf::uint64>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE :
        reflection->AddDouble(pmsg,field,value.toDouble());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT :
        reflection->AddFloat(pmsg,field,static_cast<float>(value.toDouble()));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL :
        reflection->AddBool(pmsg,field,value.toBool());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM :
        /* If the value does not correspond to a known enum value ...*/
        reflection->AddEnumValue(pmsg,field,value.toInt());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING :
        reflection->AddString(pmsg,field,value.toString().toStdString());
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE :
    {
        if ( !value.isObject() ) //嵌套必须对应object
        {
            m_str_err = EC_FIELD_NOT_MATCH % QString(field->full_name().c_str()) % " expect object";;
            return false;
        }
        google::protobuf::Message *sub_msg = reflection->AddMessage( pmsg,field );
        json_object_to_pb( value.toObject(),sub_msg );

    }break;
    default:
        m_str_err = QString("unknow type when add field:%1").arg(field->full_name().c_str());
        return false;
        break;
    }

    return true;
}

bool CProtoc::json_object_to_pb( const QJsonObject &jo, google::protobuf::Message *pmsg )
{
    pmsg->Clear();/* clear old data,not memory it holds */

    const google::protobuf::Descriptor *descriptor = pmsg->GetDescriptor();
    const google::protobuf::FieldDescriptor *field = NULL;

    QJsonObject::const_iterator itr = jo.constBegin();
    while ( itr != jo.end() )
    {
        QString _key = itr.key();
        const QJsonValue &_value = itr.value();

        field = descriptor->FindFieldByName( _key.toStdString() );
        if ( !field )
        {
            m_str_err = EC_NO_FIELD % _key;
            return false;
        }

        if ( field->is_repeated() ) //数组
        {
            if ( !_value.isArray() ) //对应数组
            {
                m_str_err = EC_FIELD_NOT_MATCH % QString(field->full_name().c_str()) % " expect array";
                return false;
            }

            const QJsonArray &array = _value.toArray();
            QJsonArray::const_iterator _itr = array.constBegin();
            while ( _itr != array.constEnd() )
            {
                if ( !add_field( pmsg,field,*_itr ) )
                    return false;

                _itr ++;
            }
        }
        else
        {
            if ( !fill_field( pmsg,field,*itr ) )
                return false;
        }

        itr ++;
    }

    return true;
}
